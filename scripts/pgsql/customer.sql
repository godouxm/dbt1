BEGIN;
TRUNCATE customer;
COPY customer FROM '/tmp/customer.data' WITH DELIMITER '>';
COMMIT;

