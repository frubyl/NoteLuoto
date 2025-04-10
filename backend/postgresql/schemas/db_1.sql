-- Установка необходимых расширений
CREATE EXTENSION IF NOT EXISTS pg_trgm;
CREATE EXTENSION IF NOT EXISTS unaccent;

-- Создание конфигурации для русского (если не существует)
DO $$
BEGIN
  IF NOT EXISTS (
    SELECT 1 FROM pg_ts_config WHERE cfgname = 'russian'
  ) THEN
    CREATE TEXT SEARCH CONFIGURATION russian (COPY = simple);
  END IF;
END
$$;

DROP SCHEMA IF EXISTS noteluoto CASCADE;
CREATE SCHEMA IF NOT EXISTS noteluoto;

CREATE TABLE IF NOT EXISTS noteluoto.users (
  id SERIAL PRIMARY KEY,
  username VARCHAR UNIQUE NOT NULL,
  password_hash VARCHAR(256) NOT NULL,
  created_at TIMESTAMPTZ NOT NULL DEFAULT NOW()
);

CREATE TABLE IF NOT EXISTS noteluoto.notes (
  id SERIAL PRIMARY KEY,
  title VARCHAR(512) NOT NULL,
  body TEXT NOT NULL,
  created_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),
  updated_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),
  user_id INTEGER NOT NULL,
  CONSTRAINT fk_notes_user FOREIGN KEY (user_id) REFERENCES noteluoto.users (id)
);

CREATE TABLE IF NOT EXISTS noteluoto.tags (
  id SERIAL PRIMARY KEY,
  user_id INTEGER NOT NULL,
  name VARCHAR(50) NOT NULL,
  CONSTRAINT fk_tags_user FOREIGN KEY (user_id) REFERENCES noteluoto.users (id)
);

CREATE TABLE IF NOT EXISTS noteluoto.note_tags (
  note_id INTEGER NOT NULL,
  tag_id INTEGER NOT NULL,
  -- Гарантирует, что заметка не будет связана с одним и тем же тегом несколько раз
  CONSTRAINT pk_note_tags PRIMARY KEY (note_id, tag_id),
  CONSTRAINT fk_note_tags_note FOREIGN KEY (note_id) REFERENCES noteluoto.notes (id),
  CONSTRAINT fk_note_tags_tag FOREIGN KEY (tag_id) REFERENCES noteluoto.tags (id)
);

CREATE TABLE IF NOT EXISTS noteluoto.checklists (
  id SERIAL PRIMARY KEY,
  title VARCHAR(256) NOT NULL,
  note_id INTEGER NOT NULL,
  created_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),
  updated_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),
  CONSTRAINT fk_checklists_note FOREIGN KEY (note_id) REFERENCES noteluoto.notes (id)
);

CREATE TABLE IF NOT EXISTS noteluoto.checklist_items (
  id SERIAL PRIMARY KEY,
  text VARCHAR(512) NOT NULL,
  completed BOOLEAN NOT NULL DEFAULT FALSE,
  checklist_id INTEGER NOT NULL,
  created_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),
  updated_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),
  CONSTRAINT fk_checklist_items_checklist FOREIGN KEY (checklist_id) REFERENCES noteluoto.checklists (id)
);

CREATE TABLE IF NOT EXISTS noteluoto.attachments (
  id SERIAL PRIMARY KEY,
  file_name VARCHAR(2048) NOT NULL,
  old_file_name VARCHAR(2048) NOT NULL,
  note_id INTEGER NOT NULL,
  CONSTRAINT fk_attachments_note FOREIGN KEY (note_id) REFERENCES noteluoto.notes (id)
);

CREATE TABLE IF NOT EXISTS noteluoto.ai_history (
  id SERIAL PRIMARY KEY,
  query TEXT NOT NULL,
  response TEXT NOT NULL,
  created_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),
  user_id INTEGER NOT NULL,
  CONSTRAINT fk_ai_history_user FOREIGN KEY (user_id) REFERENCES noteluoto.users (id)
);



-- Триггер для удаления неиспользуемых тегов
CREATE OR REPLACE FUNCTION remove_unused_tag()
RETURNS TRIGGER AS $$
BEGIN
    -- Проверяем, остались ли другие заметки, связанные с тегом
    IF NOT EXISTS (
        SELECT 1
        FROM noteluoto.note_tags
        WHERE tag_id = OLD.tag_id
    ) THEN
        -- Удаляем тег, если связей больше нет
        DELETE FROM noteluoto.tags
        WHERE id = OLD.tag_id;
    END IF;

    RETURN OLD;
END;
$$ LANGUAGE plpgsql;

-- Привязка триггера к таблице note_tags
CREATE TRIGGER delete_note_tag_trigger
AFTER DELETE ON noteluoto.note_tags
FOR EACH ROW
EXECUTE FUNCTION remove_unused_tag();



-- Триггер для обновления поля updated_at у чеклиста при изменении его пунктов
CREATE OR REPLACE FUNCTION update_checklist_timestamp()
RETURNS TRIGGER AS $$
BEGIN
  -- Для вставки или обновления, установим updated_at чеклиста как у item
  IF TG_OP = 'INSERT' OR TG_OP = 'UPDATE' THEN
    UPDATE noteluoto.checklists
    SET updated_at = NEW.updated_at
    WHERE id = NEW.checklist_id;
  END IF;

  -- Для удаления, установим updated_at чеклиста как у item (OLD)
  IF TG_OP = 'DELETE' THEN
    UPDATE noteluoto.checklists
    SET updated_at = OLD.updated_at
    WHERE id = OLD.checklist_id;
  END IF;

  RETURN NEW;
END;
$$ LANGUAGE plpgsql;

-- Привязка триггера к таблице checklist_items
CREATE TRIGGER trigger_update_checklist_timestamp
AFTER INSERT OR UPDATE OR DELETE ON noteluoto.checklist_items
FOR EACH ROW
EXECUTE FUNCTION update_checklist_timestamp();



-- Триггер для установки поля updated_at у заметки как у чеклиста при его изменении
CREATE OR REPLACE FUNCTION update_note_timestamp()
RETURNS TRIGGER AS $$
BEGIN
  UPDATE noteluoto.notes
  SET updated_at = NEW.updated_at
  WHERE id = NEW.note_id;
  RETURN NEW;
END;
$$ LANGUAGE plpgsql;

-- Привязка триггера к таблице checklists
CREATE TRIGGER trigger_update_note_timestamp
AFTER UPDATE ON noteluoto.checklists
FOR EACH ROW
WHEN (NEW.updated_at <> OLD.updated_at) 
EXECUTE FUNCTION update_note_timestamp();



-- Триггер для обновления поля updated_at у заметки при работы с вложениями
CREATE OR REPLACE FUNCTION update_note_updated_at_on_attachment_change()
RETURNS TRIGGER AS $$
BEGIN
  UPDATE noteluoto.notes
  SET updated_at = CURRENT_TIMESTAMP
  WHERE id = NEW.note_id;  
  IF TG_OP = 'DELETE' THEN
    UPDATE noteluoto.notes
    SET updated_at = CURRENT_TIMESTAMP
    WHERE id = OLD.note_id;
  END IF;
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;


-- Привязка триггера к таблице attachments
CREATE TRIGGER trigger_update_note_updated_at_on_attachment_insert
AFTER INSERT OR DELETE ON noteluoto.attachments
FOR EACH ROW
EXECUTE FUNCTION update_note_updated_at_on_attachment_change();


-- Изменение внешнего ключа, чтобы использовать каскадное удаление
-- Когда удаляем чеклист, удаляются все его пункты
ALTER TABLE noteluoto.checklist_items
DROP CONSTRAINT fk_checklist_items_checklist,
ADD CONSTRAINT fk_checklist_items_checklist
FOREIGN KEY (checklist_id) REFERENCES noteluoto.checklists(id)
ON DELETE CASCADE;

-- Изменение внешнего ключа, чтобы использовать каскадное удаление
-- Когда удаляется заметка, удаляются все её вложения
ALTER TABLE noteluoto.attachments
DROP CONSTRAINT fk_attachments_note,
ADD CONSTRAINT fk_attachments_note
FOREIGN KEY (note_id) REFERENCES noteluoto.notes(id)
ON DELETE CASCADE;

-- Изменение внешнего ключа, чтобы использовать каскадное удаление
-- Когда удаляется заметка, удаляются все её связи с тегами
ALTER TABLE noteluoto.note_tags
DROP CONSTRAINT fk_note_tags_note,
ADD CONSTRAINT fk_note_tags_note
FOREIGN KEY (note_id) REFERENCES noteluoto.notes(id)
ON DELETE CASCADE;

-- Изменение внешнего ключа, чтобы использовать каскадное удаление
-- Когда удаляется заметка, удаляются все её чеклисты
ALTER TABLE noteluoto.checklists
DROP CONSTRAINT fk_checklists_note,
ADD CONSTRAINT fk_checklists_note
FOREIGN KEY (note_id) REFERENCES noteluoto.notes(id)
ON DELETE CASCADE;


-- Индексирование
-- Индекс на note_id в таблице note_tags для ускорения добавления/удаления тегов
CREATE INDEX idx_note_tags_note_id ON noteluoto.note_tags (note_id);

-- Индекс на tag_id в таблице note_tags для ускорения поиска заметок по тегу
CREATE INDEX idx_note_tags_tag_id ON noteluoto.note_tags (tag_id);

-- Индекс на user_id в таблице notes для ускорения поиска заметок по пользователю
CREATE INDEX idx_notes_user_id ON noteluoto.notes (user_id);

-- Индекс на note_id в таблице checklists для ускорения поиска чеклистов для заметки
CREATE INDEX idx_checklists_note_id ON noteluoto.checklists (note_id);

-- Индекс на note_id в таблице attachments для ускорения поиска вложений для заметки
CREATE INDEX idx_attachments_note_id ON noteluoto.attachments (note_id);

-- Индекс на user_id в таблице ai_history для ускорения поиска запросов ИИ по пользователю
CREATE INDEX idx_ai_history_user_id ON noteluoto.ai_history (user_id);

CREATE INDEX idx_notes_fts_ru ON noteluoto.notes 
  USING gin(to_tsvector('russian', title || ' ' || body));
CREATE INDEX idx_notes_fts_en ON noteluoto.notes 
  USING gin(to_tsvector('english', title || ' ' || body));