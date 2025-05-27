#pragma once

#include "Common.h"


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
    }

}