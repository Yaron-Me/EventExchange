CREATE TABLE stocks (
    id TEXT PRIMARY KEY,
    name TEXT NOT NULL,
    description TEXT,
    settled INTEGER NOT NULL DEFAULT 0 CHECK(settled IN (0, 1)),
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE shares (
    id TEXT PRIMARY KEY,
    name TEXT NOT NULL,
    stock_id TEXT NOT NULL,
    type TEXT NOT NULL CHECK (type IN ('YES', 'NO')),
    FOREIGN KEY (stock_id) REFERENCES stocks(id) ON DELETE CASCADE
);

CREATE TABLE users (
    id TEXT PRIMARY KEY,
    name TEXT UNIQUE NOT NULL,
    password TEXT NOT NULL,
    balance INTEGER NOT NULL
);

CREATE TABLE user_holdings (
    user_id TEXT NOT NULL,
    share_id TEXT NOT NULL,
    quantity INTEGER NOT NULL,
    PRIMARY KEY (user_id, share_id),
    FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE,
    FOREIGN KEY (share_id) REFERENCES shares(id) ON DELETE CASCADE
);

CREATE TABLE comments (
    id TEXT PRIMARY KEY,
    user_id TEXT NOT NULL,
    stock_id TEXT NOT NULL,
    comment TEXT NOT NULL,
    FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE,
    FOREIGN KEY (stock_id) REFERENCES stocks(id) ON DELETE CASCADE
);

CREATE TABLE transactions (
    id TEXT PRIMARY KEY,
    user_id TEXT NOT NULL,
    share_id TEXT NOT NULL,
    transaction_type TEXT NOT NULL CHECK (transaction_type IN ('BUY', 'SELL')),
    quantity INTEGER NOT NULL,
    price INTEGER NOT NULL,
    timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE,
    FOREIGN KEY (share_id) REFERENCES shares(id) ON DELETE CASCADE
);

CREATE INDEX idx_shares_stock_id ON shares(stock_id);
CREATE INDEX idx_user_holdings_user_id ON user_holdings(user_id);
CREATE INDEX idx_user_holdings_share_id ON user_holdings(share_id);
CREATE INDEX idx_comments_user_id ON comments(user_id);
CREATE INDEX idx_comments_stock_id ON comments(stock_id);
CREATE INDEX idx_transactions_user_id ON transactions(user_id);
CREATE INDEX idx_transactions_share_id ON transactions(share_id);
