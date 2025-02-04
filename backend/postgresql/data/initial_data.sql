INSERT INTO noteluoto.users (username, password_hash)
VALUES ('frubyl', '$2b$10$t4Sw0JCBDfIuYywjloaHoOQQBe8TlpHQXY4K.9U.keRbM0Qrs8H.e');


INSERT INTO noteluoto.ai_history (query, response, user_id, created_at)
VALUES ('First query', 'First response', 1, '2025-03-10 12:30:00'),
       ('Second query', 'Second response', 1, '2025-03-10 13:30:00'),
       ('Third query', 'Third response', 1, '2025-03-10 14:30:00');


INSERT INTO noteluoto.notes (title, body, user_id, created_at, updated_at)
VALUES  ('title', 'body', 1, '2025-03-10 12:30:00', '2025-03-10 12:30:00');

INSERT INTO noteluoto.tags (user_id, name)
VALUES (1, 'tag1'),
       (1, 'tag2');

INSERT INTO noteluoto.note_tags (note_id, tag_id)
VALUES (1, 2);

INSERT INTO noteluoto.checklists (note_id, title)
VALUES (1, 'title');

INSERT INTO noteluoto.checklist_items (checklist_id, text)
VALUES (1, 'item1');