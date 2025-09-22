CREATE SCHEMA IF NOT EXISTS allocation;

-- Таблица позиций заказов
CREATE TABLE allocation.order_lines (
    id SERIAL PRIMARY KEY,
    sku VARCHAR(255) NOT NULL,
    qty INTEGER NOT NULL,
    orderid VARCHAR(255) NOT NULL
);

-- Таблица агрегатов продуктов
CREATE TABLE allocation.products (
    sku VARCHAR(255) PRIMARY KEY,
    version_number BIGINT NOT NULL DEFAULT 0
);

-- Таблица партий заказов
CREATE TABLE allocation.batches (
    id SERIAL PRIMARY KEY,
    reference VARCHAR(255) UNIQUE NOT NULL,
    sku VARCHAR(255) NOT NULL REFERENCES allocation.products(sku),
    _purchased_quantity INTEGER NOT NULL,
    eta DATE
);

-- Таблица распределений (связь OrderLine -> Batch)
CREATE TABLE allocation.allocations (
    id SERIAL PRIMARY KEY,
    orderline_id INTEGER NOT NULL REFERENCES allocation.order_lines(id),
    batch_id INTEGER NOT NULL REFERENCES allocation.batches(id)
);

-- Модель чтения (SQRC)
CREATE TABLE allocation.allocations_view (
    orderid VARCHAR(255),
    sku VARCHAR(255),
    batchref VARCHAR(255)
);
