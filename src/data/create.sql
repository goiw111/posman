CREATE TABLE IF NOT EXISTS domains (
  id 				INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  parent_domain_id		INTEGER,
  name 				varchar(15) NOT NULL,
  info 				varchar(30),

  FOREIGN KEY(parent_domain_id)
  REFERENCES domains(id)


);

REPLACE INTO domains (id,name) Values (1,'jomla');
REPLACE INTO domains (id,name) Values (2,'detai');


CREATE TABLE IF NOT EXISTS customer (
  id 			INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  full_name 	varchar(15),
  address 		varchar(15),
  domain_id 	INTEGER,
  created_at 	DATETIME DEFAULT CURRENT_TIMESTAMP,
  description 	varchar(30),
  phone 		varchar(30),

  FOREIGN KEY (domain_id)
  REFERENCES  domains(id)
);

CREATE TABLE IF NOT EXISTS category (
  "id" 		INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  "name" 	varchar(15),
  "info" 	varchar(30)
);

CREATE TABLE IF NOT EXISTS suppliers (
  id	 		INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  full_name 	varchar(15),
  phone 		varchar(15)
);

CREATE TABLE IF NOT EXISTS brands (
  id 		INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  name 		varchar(15),
  info 		varchar(30),
  sup_id 	INTEGER,

  FOREIGN KEY (sup_id)
  REFERENCES  suppliers(id)
);

CREATE TABLE IF NOT EXISTS product (
  id 		INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  full_name 	varchar(15),
  barcode 	varchar(15),
  category_id 	INTEGER,
  unit_in_pack 	INTEGER,
  brand_id 	INTEGER,

  FOREIGN KEY (category_id)
  REFERENCES  category(id),

  FOREIGN KEY (brand_id)
  REFERENCES  brands(id)
);

CREATE TABLE IF NOT EXISTS stock (
  id 			INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  prod_id 		INTEGER,
  quantity 		INTEGER,
  P_date 		DATETIME,
  E_date 		DATETIME,
  buy_price 		REAL,

  FOREIGN KEY (prod_id)
  REFERENCES  product(id)
);

CREATE TABLE IF NOT EXISTS prices (
  id 			INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  domain_id 		INTEGER,
  value 		REAL,
  stock_id 		INTEGER,

  FOREIGN KEY (domain_id)
  REFERENCES  domains(id),

  FOREIGN KEY (stock_id)
  REFERENCES  stock(id)
);

CREATE TABLE IF NOT EXISTS orders (
  id 			INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  cust_id 		INTEGER,
  status 		INT,
  order_date 		DATETIME DEFAULT CURRENT_TIMESTAMP,
  order_req_date 	DATETIME,
  order_shipped_date 	DATETIME,

  FOREIGN KEY (cust_id)
  REFERENCES  customer(id)
);

CREATE TABLE IF NOT EXISTS order_items (
  id 			INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  order_id 		INTEGER,
  stock_id 		INTEGER,
  quantity 		INTEGER,
  price 		FLOAT,

  FOREIGN KEY (order_id)
  REFERENCES  orders(id),

  FOREIGN KEY (stock_id)
  REFERENCES  stock(id)
);

CREATE TABLE IF NOT EXISTS payment (
  id 		INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  cust_id 	INTEGER,
  payment_date 	DATETIME DEFAULT CURRENT_TIMESTAMP,
  payer 	varchar(15),

  FOREIGN KEY (cust_id)
  REFERENCES  customer(id)
);

CREATE TABLE IF NOT EXISTS payment_cheque (
  id            INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  payment_id    INTEGER,
  the_value     FLOAT,
  code          INTEGER,
  full_name     varchar(15),
  date_for      DATETIME,
  pay_in        varchar(15),
  pay_for       varchar(15),

  FOREIGN KEY (payment_id)
  REFERENCES  payment(id)
);

CREATE TABLE IF NOT EXISTS payment_cash (
  id 		INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  pay_id 	INTEGER,
  value 	FLOAT,

  FOREIGN KEY (pay_id)
  REFERENCES  payment(id)
);
