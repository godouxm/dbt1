BEGIN;
TRUNCATE orders;
COPY orders FROM '/tmp/orders.data' WITH DELIMITER '>';
COMMIT;
