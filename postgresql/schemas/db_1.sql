CREATE EXTENSION IF NOT EXISTS "uuid-ossp";

DROP SCHEMA IF EXISTS cooplan CASCADE;

CREATE SCHEMA IF NOT EXISTS cooplan;

CREATE TABLE IF NOT EXISTS cooplan.users (
    id SERIAL PRIMARY KEY,
    username TEXT UNIQUE NOT NULL,
    password TEXT NOT NULL,
    contact TEXT DEFAULT '+998901112233',
    image TEXT DEFAULT 'https://images.unsplash.com/photo-1467810563316-b5476525c0f9?q=80&w=2069&auto=format&fit=crop&ixlib=rb-4.0.3&ixid=M3wxMjA3fDB8MHxwaG90by1wYWdlfHx8fGVufDB8fHx8fA%3D%3D'
);

CREATE TABLE IF NOT EXISTS cooplan.auth_sessions (
                                                     id TEXT PRIMARY KEY DEFAULT uuid_generate_v4(),
    user_id INT NOT NULL,
    foreign key(user_id) REFERENCES cooplan.users(id)
    );

CREATE TABLE IF NOT EXISTS cooplan.events (
    id SERIAL PRIMARY KEY,
    title TEXT NOT NULL,
    description TEXT,
    organizer_id BIGINT NOT NULL REFERENCES cooplan.users(id),
    is_active BOOLEAN DEFAULT true,
    members_limit INT,
    start_datetime TIMESTAMP,
    finish_datetime TIMESTAMP,
    registration_deadline TIMESTAMP,
    latitude DOUBLE PRECISION,
    longitude DOUBLE PRECISION,
    image_url TEXT NOT NULL
    );

CREATE TYPE status_enum AS ENUM ('pending', 'accepted', 'rejected');

CREATE TABLE IF NOT EXISTS cooplan.event_participant (
    id SERIAL PRIMARY KEY,
    event_id INT NOT NULL REFERENCES cooplan.events(id) ON DELETE CASCADE,
    user_id INT NOT NULL REFERENCES cooplan.users(id) ON DELETE CASCADE,
    status status_enum DEFAULT 'pending',
    UNIQUE(event_id, user_id)
    );