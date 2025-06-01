#pragma once

#include "DbTables.h"


namespace Allocation::Adapters::Database
{

    void InitDatabase(Poco::Data::Session& session)
    {
        session << R"(
            CREATE TABLE IF NOT EXISTS order_lines (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                sku VARCHAR(255),
                qty INTEGER NOT NULL,
                orderid VARCHAR(255)
            )
        )", Poco::Data::Keywords::now;

        session << R"(
            CREATE TABLE IF NOT EXISTS batches (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                reference VARCHAR(255),
                sku VARCHAR(255),
                _purchased_quantity INTEGER NOT NULL,
                eta DATE NULL
            )
        )", Poco::Data::Keywords::now;

        session << R"(
            CREATE TABLE IF NOT EXISTS allocations (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                orderline_id INTEGER,
                batch_id INTEGER,
                FOREIGN KEY(orderline_id) REFERENCES order_lines(id),
                FOREIGN KEY(batch_id) REFERENCES batches(id)
            )
        )", Poco::Data::Keywords::now;
    }
}