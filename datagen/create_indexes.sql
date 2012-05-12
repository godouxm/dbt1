ALTER TABLE author ADD CONSTRAINT author_pkey PRIMARY KEY (a_id);
ALTER TABLE country ADD CONSTRAINT country_pkey PRIMARY KEY (co_id);
ALTER TABLE address ADD CONSTRAINT address_pkey PRIMARY KEY (addr_id);
ALTER TABLE customer ADD CONSTRAINT customer_pkey PRIMARY KEY (c_id);
ALTER TABLE item ADD CONSTRAINT item_pkey PRIMARY KEY (i_id);
ALTER TABLE stock ADD CONSTRAINT stock_pkey PRIMARY KEY (st_i_id); -- pg2 additional alter
ALTER TABLE orders ADD CONSTRAINT orders_pkey PRIMARY KEY (o_id);
ALTER TABLE order_line ADD CONSTRAINT order_line_pkey PRIMARY KEY (ol_o_id, ol_id);
ALTER TABLE cc_xacts ADD CONSTRAINT cc_xacts_pkey PRIMARY KEY (cx_o_id);
ALTER TABLE shopping_cart ADD CONSTRAINT shopping_cart_pkey PRIMARY KEY (sc_id);
ALTER TABLE shopping_cart_line ADD CONSTRAINT shopping_cart_line_pkey PRIMARY KEY (scl_sc_id, scl_i_id);

CREATE INDEX i_i_subject ON item (i_subject);
CREATE INDEX i_i_a_id ON item (i_a_id);
CREATE INDEX i_address_table ON address (addr_street1, addr_street2, addr_city, addr_state, addr_zip, addr_co_id);
CREATE INDEX i_c_uname ON customer (c_uname);
CREATE INDEX i_o_c_id ON orders (o_c_id);
-- CREATE INDEX o_o_date ON orders (o_date);
