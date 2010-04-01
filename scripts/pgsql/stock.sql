BEGIN;
TRUNCATE stock;
COPY stock FROM '/tmp/stock.data' WITH DELIMITER '>';
COMMIT;
