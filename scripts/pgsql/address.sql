BEGIN;
TRUNCATE address;
COPY address FROM '/tmp/address.data' WITH DELIMITER '>';
COMMIT ;
