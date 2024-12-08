INSERT INTO cooplan.users (username, password)
VALUES ('tester', 'tester')
ON CONFLICT (username) DO NOTHING;

INSERT INTO cooplan.events (
    title,
    description,
    organizer_id,
    is_active,
    members_limit,
    start_datetime,
    finish_datetime,
    registration_deadline,
    latitude,
    longitude,
    image_url
) VALUES (
    'Annual Tech Conference',
    'A conference bringing together tech enthusiasts and professionals.',
    1,
    true,
    500,
    '2025-05-01 09:00:00',
    '2025-05-01 17:00:00',
    '2025-04-25 23:59:59',
    69.3451,
    41.2995,
    'https://example.com/image.jpg'
);

INSERT INTO cooplan.event_participant (
    event_id,
    user_id
) VALUES (
    1,
    1
);

INSERT INTO cooplan.auth_sessions (
    id,
    user_id
) VALUES (
    'asdf',
    1
);

INSERT INTO cooplan.users(
    id, username, contact, image, password
)values(
    123,'test', 'test@example.com', 'photo_url', 'abcd'
);

-- INSERT INTO cooplan.events(
--     title, organizer_id, image_url, description
-- )VALUES (
--     'Event 1', 123, 'ask', 'a'
-- );
--
-- INSERT INTO cooplan.event_participant(
--     id, event_id, user_id, status
-- )VALUES(
--         2, '120', '10', 'accepted'
--        );
INSERT INTO cooplan.event_participant (
    event_id,
    user_id,
    status
) VALUES (
             1,
             123,
            'accepted'
         );



