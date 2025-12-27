PRAGMA foreign_keys = ON;

CREATE TABLE IF NOT EXISTS users (
    user_id        TEXT PRIMARY KEY,
    username       TEXT NOT NULL UNIQUE,
    password_hash  TEXT NOT NULL,
    national_id    TEXT NOT NULL UNIQUE
);

CREATE TABLE IF NOT EXISTS events (
    event_id       TEXT PRIMARY KEY,
    event_name     TEXT NOT NULL,
    start_time     TEXT NOT NULL,
    end_time       TEXT NOT NULL,

    CHECK (start_time >= '18:00'),
    CHECK (end_time <= '23:00'),
    CHECK (start_time < end_time)
);

CREATE TABLE IF NOT EXISTS ticket_categories (
    category_id    TEXT PRIMARY KEY,
    name           TEXT NOT NULL UNIQUE
);

CREATE TABLE IF NOT EXISTS event_ticket_categories (
    event_id       TEXT NOT NULL,
    category_id    TEXT NOT NULL,
    price          REAL NOT NULL CHECK (price >= 0),
    capacity       INTEGER NOT NULL CHECK (capacity > 0),

    PRIMARY KEY (event_id, category_id),

    FOREIGN KEY (event_id)
        REFERENCES events(event_id)
        ON DELETE CASCADE,

    FOREIGN KEY (category_id)
        REFERENCES ticket_categories(category_id)
        ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS reservations (
    reservation_id   TEXT PRIMARY KEY,
    user_id          TEXT NOT NULL,
    event_id         TEXT NOT NULL,
    category_id      TEXT NOT NULL,
    national_id      TEXT NOT NULL,
    ticket_count     INTEGER NOT NULL CHECK (ticket_count > 0),
    status           TEXT NOT NULL,
    total_price      REAL NOT NULL CHECK (total_price >= 0),
    reservation_time TEXT NOT NULL,

    FOREIGN KEY (user_id)
        REFERENCES users(user_id)
        ON DELETE CASCADE,

    FOREIGN KEY (event_id, category_id)
        REFERENCES event_ticket_categories(event_id, category_id)
        ON DELETE RESTRICT
);

CREATE TABLE IF NOT EXISTS payments (
    payment_id       TEXT PRIMARY KEY,
    reservation_id   TEXT NOT NULL,
    payment_date     TEXT NOT NULL,
    national_id      TEXT NOT NULL,
    amount           REAL NOT NULL CHECK (amount > 0),

    FOREIGN KEY (reservation_id)
        REFERENCES reservations(reservation_id)
        ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS refunds (
    refund_id        TEXT PRIMARY KEY,
    reservation_id   TEXT NOT NULL,
    refund_date      TEXT NOT NULL,
    national_id      TEXT NOT NULL,
    amount           REAL NOT NULL CHECK (amount > 0),

    FOREIGN KEY (reservation_id)
        REFERENCES reservations(reservation_id)
        ON DELETE CASCADE
);

CREATE INDEX IF NOT EXISTS idx_reservations_event 
  ON reservations(event_id);
  
CREATE INDEX IF NOT EXISTS idx_reservations_user 
  ON reservations(user_id);

CREATE INDEX IF NOT EXISTS idx_users_username 
  ON users(username);

CREATE INDEX IF NOT EXISTS idx_payments_reservation 
  ON payments(reservation_id);

CREATE INDEX IF NOT EXISTS idx_refunds_reservation
  ON refunds(reservation_id);
  
CREATE INDEX IF NOT EXISTS idx_res_status_time 
  ON reservations(status, reservation_time);
