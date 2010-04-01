BEGIN;
TRUNCATE order_line;
COPY order_line FROM '/tmp/order_line.data' USING DELIMITERS '>';
COMMIT;
