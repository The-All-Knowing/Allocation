-- Включаем расширение для UUID
CREATE EXTENSION IF NOT EXISTS "uuid-ossp";

-- Создаём схему для изоляции объектов
CREATE SCHEMA IF NOT EXISTS allocation;

-- Таблица продуктов
CREATE TABLE IF NOT EXISTS allocation.products (
    sku VARCHAR(255) PRIMARY KEY NOT NULL,
    version_number INTEGER NOT NULL DEFAULT 0
);

-- Таблица строк заказов
CREATE TABLE IF NOT EXISTS allocation.order_lines (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    sku VARCHAR(255) NOT NULL,
    qty INTEGER NOT NULL,
    orderid VARCHAR(255) NOT NULL,
    CONSTRAINT uniq_order UNIQUE (orderid, sku),
    CONSTRAINT fk_product FOREIGN KEY (sku) REFERENCES allocation.products(sku) ON DELETE CASCADE
);

-- Таблица партий (batch)
CREATE TABLE IF NOT EXISTS allocation.batches (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    reference VARCHAR(255) NOT NULL,
    sku VARCHAR(255) NOT NULL,
    purchased_quantity INTEGER NOT NULL,
    eta DATE,
    CONSTRAINT fk_product FOREIGN KEY (sku) REFERENCES allocation.products(sku) ON DELETE CASCADE
);

-- Таблица аллокаций
CREATE TABLE IF NOT EXISTS allocation.allocations (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    orderline_id UUID NOT NULL,
    batch_id UUID NOT NULL,
    CONSTRAINT fk_orderline FOREIGN KEY (orderline_id) REFERENCES allocation.order_lines(id) ON DELETE CASCADE,
    CONSTRAINT fk_batch FOREIGN KEY (batch_id) REFERENCES allocation.batches(id) ON DELETE CASCADE
);

-- Индексы для оптимизации поиска
CREATE INDEX IF NOT EXISTS idx_batches_sku ON allocation.batches(sku);
CREATE INDEX IF NOT EXISTS idx_orderlines_sku ON allocation.order_lines(sku);

-- Представление (view) для просмотра аллокаций
CREATE OR REPLACE VIEW allocation.allocations_view AS
SELECT 
    o.orderid,
    o.sku,
    b.reference AS batchref
FROM allocation.allocations a
JOIN allocation.order_lines o ON a.orderline_id = o.id
JOIN allocation.batches b ON a.batch_id = b.id;
