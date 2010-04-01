BEGIN;
TRUNCATE author;
COPY author FROM '/tmp/author.data' WITH DELIMITER '>';
COMMIT;
