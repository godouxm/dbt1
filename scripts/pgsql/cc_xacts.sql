BEGIN;
TRUNCATE cc_xacts;
COPY cc_xacts FROM '/tmp/cc_xacts.data' WITH DELIMITER '>';
COMMIT;
