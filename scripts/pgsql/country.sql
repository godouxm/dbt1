BEGIN;
TRUNCATE country;
COPY country FROM '/tmp/country.data' WITH DELIMITER '>';
COMMIT;
