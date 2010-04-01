
CREATE TABLE author (
  a_id integer -- numeric(10)
, a_fname varchar(20)
, a_lname varchar(20)
, a_mname varchar(20)
, a_dob date
, a_bio varchar(500)
) DISTRIBUTE BY REPLICATION;

CREATE TABLE country (
  co_id smallint -- numeric(4)
, co_name varchar(50)
, co_exchange numeric(12, 6)
, co_currency varchar(18)
) DISTRIBUTE BY REPLICATION;

CREATE TABLE address (
  addr_id integer -- numeric(10)
, addr_street1 varchar(40)
, addr_street2 varchar(40)
, addr_city varchar(30)
, addr_state varchar(20)
, addr_zip varchar(10)
, addr_co_id smallint -- numeric(4)
, addr_c_id integer -- pgxc additional column
) DISTRIBUTE BY HASH (addr_c_id);

CREATE TABLE customer (
  c_id integer -- numeric(10)
, c_uname varchar(20)
, c_passwd varchar(20)
, c_fname varchar(15)
, c_lname varchar(15)
, c_addr_id integer -- numeric(10)
, c_phone varchar(16)
, c_email varchar(50)
, c_since date
, c_last_visit date
, c_login timestamp
, c_expiration timestamp
, c_discount numeric(5, 2)
, c_balance numeric(17, 2)
, c_ytd_pmt numeric(17, 2)
, c_birthdate date
, c_data varchar(500)
) DISTRIBUTE BY HASH (c_id);

CREATE TABLE item (
  i_id integer -- numeric(10)
, i_title varchar(60)
, i_a_id integer -- numeric(10)
, i_pub_date date
, i_publisher varchar(60)
, i_subject varchar(60)
, i_desc varchar(500)
, i_related1 integer -- numeric(10)
, i_related2 integer -- numeric(10)
, i_related3 integer -- numeric(10)
, i_related4 integer -- numeric(10)
, i_related5 integer -- numeric(10)
, i_thumbnail integer -- numeric(8)
, i_image integer -- numeric(8)
, i_srp numeric(17, 2)
, i_cost numeric(17, 2)
, i_avail date
-- , i_stock smallint -- numeric(4) -- moved to a separate table
, i_isbn char(13)
, i_page smallint -- numeric(4)
, i_backing varchar(15)
, i_dimensions varchar(25)
) DISTRIBUTE BY REPLICATION;

CREATE TABLE stock ( -- pgxc additional table
  st_i_id integer -- numeric(10)
, st_stock smallint
) DISTRIBUTE BY HASH (st_i_id);

CREATE TABLE orders (
  o_id integer -- numeric(10)
, o_c_id integer -- numeric(10)
, o_date timestamp
, o_sub_total numeric(17, 2)
, o_tax numeric(17, 2)
, o_total numeric(17, 2)
, o_ship_type varchar(10)
, o_ship_date timestamp
, o_bill_addr_id integer -- numeric(10)
, o_ship_addr_id integer -- numeric(10)
, o_status varchar(15)
) DISTRIBUTE BY HASH (o_c_id);

CREATE TABLE order_line (
  ol_id smallint -- numeric(3)
, ol_o_id integer -- numeric(10)
, ol_i_id integer -- numeric(10)
, ol_qty smallint -- numeric(3)
, ol_discount numeric(5, 2)
, ol_comments varchar(100)
, ol_c_id integer -- pgxc additional column
) DISTRIBUTE BY HASH (ol_c_id);

CREATE TABLE cc_xacts (
  cx_o_id integer -- numeric(10)
, cx_type varchar(10)
, cx_num bigint -- numeric(16)
, cx_name varchar(31)
, cx_expiry date
, cx_auth_id char(15)
, cx_xact_amt numeric(17, 2)
, cx_xact_date timestamp
, cx_co_id smallint -- numeric(4)
, cx_c_id integer -- pgxc additional column
) DISTRIBUTE BY HASH (cx_c_id);

CREATE TABLE shopping_cart (
  sc_id integer -- numeric(10)
, sc_c_id integer -- numeric(10)
, sc_date timestamp
, sc_sub_total numeric(17, 2)
, sc_tax numeric(17, 2)
, sc_ship_cost numeric(17, 2)
, sc_total numeric(17, 2)
, sc_c_fname varchar(15)
, sc_c_lname varchar(15)
, sc_c_discount numeric(5, 2)
) DISTRIBUTE BY HASH (sc_id);

CREATE TABLE shopping_cart_line (
  scl_sc_id integer -- numeric(10)
, scl_i_id integer -- numeric(10)
, scl_qty smallint -- numeric(3)
, scl_cost numeric(17, 2)
, scl_srp numeric(17, 2)
, scl_title varchar(60)
, scl_backing varchar(15)
, scl_c_id integer -- pgxc additional column
) DISTRIBUTE BY HASH (scl_sc_id);


GRANT ALL PRIVILEGES ON TABLE
  address
, author
, cc_xacts
, country
, customer
, item
, stock
, order_line
, orders
, shopping_cart
, shopping_cart_line
TO PUBLIC;

