-- ----------------
-- create user
-- ----------------
CREATE USER 'muggle'@'%' IDENTIFIED BY 'wsz123';
CREATE USER 'muggle'@'localhost' IDENTIFIED BY 'wsz123';

GRANT ALL PRIVILEGES ON *.* TO 'muggle'@'%';
GRANT ALL PRIVILEGES ON *.* TO 'muggle'@'localhost';

-- ----------------
-- ban root remote login
-- ----------------
DELETE FROM mysql.user WHERE User='root' AND Host NOT IN ('localhost', '127.0.0.1', '::1');
