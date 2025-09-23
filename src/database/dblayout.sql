CREATE TABLE events (
    id TEXT PRIMARY KEY,
    name TEXT NOT NULL,
    description TEXT,
    issued INTEGER NOT NULL DEFAULT 0,
    settled INTEGER NOT NULL DEFAULT 0 CHECK(settled IN (0, 1)),
    created_at TEXT DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE shares (
    id TEXT PRIMARY KEY,
    name TEXT NOT NULL,
    event_id TEXT NOT NULL,
    type TEXT NOT NULL CHECK (type IN ('YES', 'NO')),
    FOREIGN KEY (event_id) REFERENCES events(id) ON DELETE CASCADE
);

CREATE TABLE users (
    id TEXT PRIMARY KEY,
    name TEXT UNIQUE NOT NULL,
    password TEXT NOT NULL,
    balance INTEGER NOT NULL,
    created_at TEXT DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE user_holdings (
    user_id TEXT NOT NULL,
    share_id TEXT NOT NULL,
    quantity INTEGER NOT NULL,
    costs INTEGER NOT NULL DEFAULT 0,
    PRIMARY KEY (user_id, share_id),
    FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE,
    FOREIGN KEY (share_id) REFERENCES shares(id) ON DELETE CASCADE
);

CREATE TABLE comments (
    id TEXT PRIMARY KEY,
    user_id TEXT NOT NULL,
    event_id TEXT NOT NULL,
    comment TEXT NOT NULL,
    FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE,
    FOREIGN KEY (event_id) REFERENCES events(id) ON DELETE CASCADE
);

CREATE TABLE transactions (
    id TEXT PRIMARY KEY,
    user_id TEXT NOT NULL,
    share_id TEXT NOT NULL,
    quantity INTEGER NOT NULL,
    price INTEGER NOT NULL,
    timestamp TEXT DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE,
    FOREIGN KEY (share_id) REFERENCES shares(id) ON DELETE CASCADE
);

CREATE TABLE finished_orders (
    id TEXT PRIMARY KEY,
    user_id TEXT,
    order_type TEXT NOT NULL CHECK (order_type IN ('BUY', 'SELL')),
    order_mode TEXT NOT NULL CHECK (order_mode IN ('MARKET', 'LIMIT')),
    event_id TEXT NOT NULL,
    share_id TEXT NOT NULL,
    quantity INTEGER NOT NULL,
    price INTEGER NOT NULL,
    filled_quantity INTEGER NOT NULL DEFAULT 0,
    transacted_value INTEGER NOT NULL DEFAULT 0,
    created_at TEXT NOT NULL,
    finished_at TEXT DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE SET NULL,
    FOREIGN KEY (event_id) REFERENCES events(id) ON DELETE CASCADE,
    FOREIGN KEY (share_id) REFERENCES shares(id) ON DELETE CASCADE
);

CREATE INDEX idx_shares_event_id ON shares(event_id);
CREATE INDEX idx_user_holdings_user_id ON user_holdings(user_id);
CREATE INDEX idx_user_holdings_share_id ON user_holdings(share_id);
CREATE INDEX idx_comments_user_id ON comments(user_id);
CREATE INDEX idx_comments_event_id ON comments(event_id);
CREATE INDEX idx_transactions_user_id ON transactions(user_id);
CREATE INDEX idx_transactions_share_id ON transactions(share_id);
CREATE INDEX idx_finished_orders_user_id ON finished_orders(user_id);
CREATE INDEX idx_finished_orders_event_id ON finished_orders(event_id);
CREATE INDEX idx_finished_orders_share_id ON finished_orders(share_id);
