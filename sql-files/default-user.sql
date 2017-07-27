-- -----------------------------------------------------------------------
-- Create By: Spell-Master SqlServe (C)
-- Build date: 2017-07-27
-- -----------------------------------------------------------------------

CREATE USER 'ragnarok'@'localhost' IDENTIFIED BY 'ragnarok';

GRANT GRANT OPTION ON *.* TO 'ragnarok'@'localhost';

GRANT SELECT, INSERT, UPDATE, DELETE, CREATE, DROP, RELOAD, SHUTDOWN, PROCESS, FILE, REFERENCES, INDEX, ALTER, SHOW DATABASES, SUPER, CREATE TEMPORARY TABLES, LOCK TABLES, EXECUTE, REPLICATION SLAVE, REPLICATION CLIENT, CREATE VIEW, SHOW VIEW, CREATE ROUTINE, ALTER ROUTINE, CREATE USER, EVENT, TRIGGER ON *.* TO 'ragnarok'@'localhost';
