-- copy dbt1 data to the cluster
copy address from '/tmp/address.data' delimiter '>';
copy author from '/tmp/author.data' delimiter '>';
copy stock from '/tmp/stock.data' delimiter '>';
copy item from '/tmp/item.data' delimiter '>';
copy country from '/tmp/country.data' delimiter '>';
copy customer from '/tmp/customer.data' delimiter '>';
copy orders from '/tmp/orders.data' delimiter '>';
copy order_line from '/tmp/order_line.data' delimiter '>';
copy cc_xacts from '/tmp/cc_xacts.data' delimiter '>';
