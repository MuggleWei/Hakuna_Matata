-- ----------------
-- create user
-- ----------------
CREATE USER 'mugglewei'@'%' IDENTIFIED BY 'wsz123';
CREATE USER 'mugglewei'@'localhost' IDENTIFIED BY 'wsz123';

GRANT ALL PRIVILEGES ON *.* TO 'mugglewei'@'%';
GRANT ALL PRIVILEGES ON *.* TO 'mugglewei'@'localhost';

-- ----------------
-- ban root remote login
-- ----------------
DELETE FROM mysql.user WHERE User='root' AND Host NOT IN ('localhost', '127.0.0.1', '::1');
