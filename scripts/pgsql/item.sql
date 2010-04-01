BEGIN;
TRUNCATE item;
COPY item FROM '/tmp/item.data' WITH DELIMITER '>';
COMMIT;
