CREATE SCHEMA IF NOT EXISTS allocation;

-- Таблица строк заказов
CREATE TABLE allocation.order_lines (
    id SERIAL PRIMARY KEY,
    sku VARCHAR(255),
    qty INTEGER NOT NULL,
    orderid VARCHAR(255)
);

-- Таблица продуктов
CREATE TABLE allocation.products (
    sku VARCHAR(255) PRIMARY KEY,
    version_number INTEGER NOT NULL DEFAULT 0
);

-- Таблица партий
CREATE TABLE allocation.batches (
    id SERIAL PRIMARY KEY,
    reference VARCHAR(255),
    sku VARCHAR(255) REFERENCES allocation.products(sku),
    _purchased_quantity INTEGER NOT NULL,
    eta DATE
);

-- Таблица аллокаций
CREATE TABLE allocation.allocations (
    id SERIAL PRIMARY KEY,
    orderline_id INTEGER REFERENCES allocation.order_lines(id),
    batch_id INTEGER REFERENCES allocation.batches(id)
);

-- Таблица/представление для аллокаций
CREATE TABLE allocation.allocations_view (
    orderid VARCHAR(255),
    sku VARCHAR(255),
    batchref VARCHAR(255)
);

