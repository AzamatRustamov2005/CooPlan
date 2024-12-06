CREATE EXTENSION IF NOT EXISTS "uuid-ossp";

DROP SCHEMA IF EXISTS cooplan CASCADE;

CREATE SCHEMA IF NOT EXISTS cooplan;

CREATE TABLE IF NOT EXISTS cooplan.users (
    id SERIAL PRIMARY KEY,
    username TEXT UNIQUE NOT NULL,
    password TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS cooplan.auth_sessions (
    id TEXT PRIMARY KEY DEFAULT uuid_generate_v4(),
    user_id INT NOT NULL,
    foreign key(user_id) REFERENCES cooplan.users(id)
);

CREATE TABLE IF NOT EXISTS cooplan.events (
    id TEXT PRIMARY KEY DEFAULT uuid_generate_v4(),
    title TEXT NOT NULL,
    description TEXT,
    organizer_id INT NOT NULL REFERENCES cooplan.users(id),
    is_active BOOLEAN DEFAULT true
    members_limit INT,
    start_datetime TIMESTAMP,
    finish_datetime TIMESTAMP,
    registration_deadline TIMESTAMP,
    location GEOGRAPHY(POINT),
    image_url TEXT NOT NULL,
    checklist TEXT[]
);

CREATE TYPE status_enum AS ENUM ('pending', 'accepted', 'rejected');

CREATE TABLE IF NOT EXISTS cooplan.event_participant (
    id TEXT DEFAULT uuid_generate_v4(),
    event_id INT NOT NULL REFERENCES cooplan.events(id) ON DELETE CASCADE,
    user_id INT NOT NULL REFERENCES cooplan.users(id) ON DELETE CASCADE,
    status status_enum DEFAULT 'pending',
    PRIMARY KEY (event_id, user_id),
    UNIQUE(event_id, user_id)
);