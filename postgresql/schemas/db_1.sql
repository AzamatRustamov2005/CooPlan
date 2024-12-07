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