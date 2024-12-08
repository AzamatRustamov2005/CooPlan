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

-- Insert test users
INSERT INTO cooplan.users (username, password)
VALUES
    ('organizer1', 'password1'),
    ('organizer2', 'password2'),
    ('participant1', 'password3'),
    ('participant2', 'password4');

-- Insert test auth_sessions (simulating user tokens)
INSERT INTO cooplan.auth_sessions (id, user_id)
VALUES
    ('session_token_organizer1', 2), -- Organizer 1's session
    ('session_token_organizer2', 3), -- Organizer 2's session
    ('session_token_participant1', 4), -- Participant 1's session
    ('session_token_participant2', 5); -- Participant 2's session

-- Insert test events organized by the users
INSERT INTO cooplan.events (
    title, description, organizer_id, is_active, members_limit, start_datetime,
    finish_datetime, registration_deadline, latitude, longitude, image_url
)
VALUES
    ('Tech Meetup', 'A meetup for tech enthusiasts', 2, true, 100,
     '2024-01-15 10:00:00', '2024-01-15 17:00:00', '2024-01-10 23:59:59', 37.7749, -122.4194, 'image1.jpg'),
    ('Charity Run', 'Annual charity marathon', 3, true, 50,
     '2024-02-20 06:00:00', '2024-02-20 12:00:00', '2024-02-15 23:59:59', 34.0522, -118.2437, 'image2.jpg'),
    ('Party', 'Yandex Party', 3, true, 20,
     '2024-12-20 07:00:00', '2024-12-20 17:00:00', '2024-12-15 23:59:59', 44.0522, -128.2437, 'image3.jpg');

-- Insert event participants with 'pending' status
INSERT INTO cooplan.event_participant (event_id, user_id, status)
VALUES
    (3, 4, 'pending'), -- Participant 1 requests to join Tech Meetup
    (3, 5, 'pending'), -- Participant 2 requests to join Tech Meetup
    (4, 4, 'accepted'), -- Participant 1 requests to join Charity Run
    (4, 5, 'pending'); -- Participant 2 requests to join Charity Run
