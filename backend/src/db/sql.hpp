#pragma once

#include <string_view>
// Запросы к базе данных
namespace nl::db::sql {
    // Получение пользователя по username
    inline constexpr std::string_view kGetUserByUsername{
        R"~(SELECT * 
            FROM noteluoto.users 
            WHERE username = $1)~"};

    // Добавление новго пользователя
    inline constexpr std::string_view kAddNewUser{
        R"~(INSERT INTO noteluoto.users (username, password_hash)
            VALUES ($1, $2);)~"};

    // Получение истории пользователя 
    inline constexpr std::string_view kGetUserHistory{
        R"~(SELECT * 
            FROM noteluoto.ai_history 
            WHERE user_id = $1 
            ORDER BY created_at DESC
            LIMIT $2
            OFFSET $3;)~"};
     
    // Создание заметки
    inline constexpr std::string_view kCreateNote{
        R"~(INSERT INTO noteluoto.notes (title, body, user_id)
            VALUES ($1, $2, $3)
            RETURNING id;)~"};

    // Получение заметки по id
    inline constexpr std::string_view kGetNote{
        R"~(SELECT title, body, created_at, updated_at
            FROM noteluoto.notes 
            WHERE id = $1;)~"};

    // Обновление заголовка заметки
    inline constexpr std::string_view kUpdateNoteTitle{
        R"~(UPDATE noteluoto.notes
            SET title = $1, 
                updated_at = NOW()
            WHERE id = $2;)~"};

    // Обновление тела заметки
    inline constexpr std::string_view kUpdateNoteBody{
        R"~(UPDATE noteluoto.notes
            SET body = $1, 
                updated_at = NOW()
            WHERE id = $2;)~"};

    // Удалить заметку
    inline constexpr std::string_view kDeleteNote{
        R"~(DELETE FROM noteluoto.notes
            WHERE id = $1;)~"};

    // Получение тега по user_id и name
    inline constexpr std::string_view kFindTagByName{
        R"~(SELECT * 
            FROM noteluoto.tags
            WHERE name = $1 
            AND user_id = $2;)~"};
    // Создание тега
    inline constexpr std::string_view kCreateTag{
        R"~(INSERT INTO noteluoto.tags (name, user_id)
            VALUES ($1, $2)
            RETURNING id;)~"};

    // Получить все теги пользователя от самых популярных к менее популярным
    inline constexpr std::string_view kGetAllTags{
        R"~(SELECT t.id, t.name
            FROM noteluoto.tags t
            LEFT JOIN noteluoto.note_tags nt ON t.id = nt.tag_id
            WHERE t.user_id = $1
            GROUP BY t.id, t.name
            ORDER BY COUNT(nt.note_id) DESC;
            )~"};
    // Получить теги заметки
    inline constexpr std::string_view kGetNoteTags{
        R"~(SELECT t.id, t.name
            FROM noteluoto.tags t
            JOIN noteluoto.note_tags nt ON t.id = nt.tag_id
            WHERE nt.note_id = $1;
            )~"};

    // Получить тег по айди 
    inline constexpr std::string_view kGetTagById{
    R"~(SELECT 1
        FROM noteluoto.tags 
        WHERE id = $1;
        )~"};

    // Проверить добавлен ли тег к заметке
    inline constexpr std::string_view kCheckTagNote{
    R"~(SELECT 1
        FROM noteluoto.note_tags 
        WHERE note_id = $1 AND tag_id = $2;
        )~"};

    // Добавить тег к заметке
    inline constexpr std::string_view kAddTagToNote{
    R"~(INSERT INTO noteluoto.note_tags (note_id, tag_id)
        VALUES ($1, $2);)~"};

    // Удалить тег из заметки
    inline constexpr std::string_view kDeleteTagFromNote{
    R"~(DELETE FROM noteluoto.note_tags
        WHERE note_id = $1 AND tag_id = $2;
        )~"};
        
    // Создать чеклист 
    inline constexpr std::string_view kCreateChecklist{
    R"~(INSERT INTO noteluoto.checklists (note_id, title)
        VALUES ($1, $2)
        RETURNING id;)~"};
    
    // Поиск чеклиста 
    inline constexpr std::string_view kGetChecklist{
    R"~(SELECT title, created_at, updated_at
        FROM noteluoto.checklists 
        WHERE id = $1;)~"};

    // Вставить пункт в чеклист
    inline constexpr std::string_view kCreateChecklistItem{
    R"~(INSERT INTO noteluoto.checklist_items (checklist_id, text)
        VALUES ($1, $2)
        RETURNING id;
        )~"};

    // Обновление статуса пункта чеклиста
    inline constexpr std::string_view kUpdateCheckListItemStatus{
        R"~(UPDATE noteluoto.checklist_items
            SET completed = $1,
                updated_at = NOW()
            WHERE id = $2;)~"};

    // Обновление текста пункта чеклиста
    inline constexpr std::string_view kUpdateCheckListItemText{
        R"~(UPDATE noteluoto.checklist_items
            SET text = $1,
                updated_at = NOW()
            WHERE id = $2;)~"};

    // Получить пункт чеклиста 
    inline constexpr std::string_view kGetChecklistItem{
    R"~(SELECT *
        FROM noteluoto.checklist_items 
        WHERE id = $1;)~"};


    // Получить все пункты чеклиста 
    inline constexpr std::string_view kGetAllChecklistItems{
    R"~(SELECT id, text, completed, updated_at, created_at
        FROM noteluoto.checklist_items 
        WHERE checklist_id = $1;
        )~"};


    // Удалить пункт чеклиста
    inline constexpr std::string_view kDeleteChecklistItem{
    R"~(DELETE FROM noteluoto.checklist_items 
        WHERE id = $1;
        )~"};

    // Удалить чеклист
    inline constexpr std::string_view kDeleteChecklist{
    R"~(DELETE FROM noteluoto.checklists
        WHERE id = $1;
        )~"};

    // Обновить заголовок чеклиста
    inline constexpr std::string_view kUpdateChecklist{
    R"~(UPDATE noteluoto.checklists
        SET title = $1,
            updated_at = NOW()
        WHERE id = $2;
        )~"};
    
    // Добавить вложение 
    inline constexpr std::string_view kAddAttachmentToNode{
        R"~(INSERT INTO noteluoto.attachments (file_name, old_file_name, note_id)
            VALUES ($1, $2, $3)
            RETURNING id;
        )~"};

    // Получить имя вложения
    inline constexpr std::string_view kGetAttachmentNames{
        R"~(SELECT file_name, old_file_name
            FROM noteluoto.attachments 
            WHERE id = $1;
        )~"};
        
    // Удалить вложение
    inline constexpr std::string_view kDeleteAttachment{
    R"~(DELETE FROM noteluoto.attachments 
        WHERE id = $1;
        )~"};

    // Получить все вложения данной заметки
    inline constexpr std::string_view kGetAllNoteAttachments{
        R"~(SELECT file_name, old_file_name
            FROM noteluoto.attachments 
            WHERE note_id = $1;
            )~"};
}