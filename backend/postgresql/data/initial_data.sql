INSERT INTO noteluoto.users (username, password_hash)
VALUES ('frubyl', '$2b$10$LIuM6TBiC8YMMWa9kvp2v.mGDLjHWyaBcZ6uKhIhESa1poB5q5YTm');


INSERT INTO noteluoto.ai_history (query, response, user_id, created_at)
VALUES ('First query', 'First response', 1, '2025-03-10 12:30:00'),
       ('Second query', 'Second response', 1, '2025-03-10 13:30:00'),
       ('Third query', 'Third response', 1, '2025-03-10 14:30:00'),
       ('Fourth query', 'Fourth response', 1, '2025-03-10 14:31:00'),
       ('Fifth query', 'Fifth response', 1, '2025-03-10 14:32:00'),
       ('Sixth query', 'Sixth response', 1, '2025-03-10 15:30:00');

INSERT INTO noteluoto.notes (title, body, user_id, created_at, updated_at)
VALUES  ('title', 'body', 1, '2025-03-10 12:30:00', '2025-03-10 12:30:00');


INSERT INTO noteluoto.notes (id, title, body, user_id, created_at, updated_at)
VALUES  (4, 'title4', 'body4', 1, '2025-03-10 12:00:00', '2025-03-10 12:00:00'),
        (5, 'title', 'body', 1, '2025-03-10 11:00:00', '2025-03-10 11:00:00'),
        (6, 'title', 'body', 1, '2025-03-10 10:00:00', '2025-03-10 10:00:00'),
        (7, 'заголовок', 'body', 1, '2025-03-10 10:00:00', '2025-03-10 10:00:00');


INSERT INTO noteluoto.tags (user_id, name)
VALUES (1, 'tag1'),
       (1, 'tag2'),
       (1, 'tag3');

INSERT INTO noteluoto.note_tags (note_id, tag_id)
VALUES (1, 2),
       (4, 3),
       (4, 2),
       (5, 3),
       (6, 3);

INSERT INTO noteluoto.checklists (note_id, title, created_at, updated_at)
VALUES (1, 'title', '2025-03-10 12:30:00', '2025-03-10 12:30:00'),
       (4, 'checklistTitle', '2025-03-10 12:30:00', '2025-03-10 12:30:00');

INSERT INTO noteluoto.checklist_items (checklist_id, text, created_at, updated_at)
VALUES (1, 'item1', '2025-03-10 12:30:00', '2025-03-10 12:30:00'), 
       (2, 'checklistItem1', '2025-03-10 12:30:00', '2025-03-10 12:30:00');


INSERT INTO noteluoto.checklists (note_id, id, title, created_at, updated_at)
VALUES (1, 99, 'title', '2025-03-10 12:30:00', '2025-03-10 12:30:00');

INSERT INTO noteluoto.checklist_items (id, checklist_id, text, created_at, updated_at)
VALUES (99, 99, 'item1', '2025-03-10 12:30:00', '2025-03-10 12:30:00');

INSERT INTO noteluoto.attachments (id, note_id, old_file_name, file_name)
VALUES  (22, 1, 'empty.txt', 'empty.txt'),  
        (20, 1, 'del.txt', 'del.txt'),  
        (23, 1, 'test.txt', 'test.txt');
