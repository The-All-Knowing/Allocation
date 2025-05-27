#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "Precompile.h"
#include "CommonFunctions.h"
#include "Adapters/Database/Include/Common.h"
#include "Adapters/Database/Include/Mappers/OrderLineMapper.h"


namespace Allocation::Infrastructure::DB
{

    using ::testing::UnorderedElementsAre;

    TEST(Database, test_orderline_mapper_can_load_lines)
    {
        Poco::Data::SQLite::Connector::registerConnector();

        Poco::Data::Session session("SQLite", ":memory:");
        Adapters::Database::InitDatabase(session);

        session << R"(INSERT INTO order_lines (orderid, sku, qty) VALUES 
                        ('order1', 'RED-CHAIR', 12),
                        ('order1', 'RED-TABLE', 13),
                        ('order2', 'BLUE-LIPSTICK', 14))", Poco::Data::Keywords::now;
        
        auto result = Adapters::Database::Mapper::OrderLineMapper(session).FindAll();

        EXPECT_EQ(result.size(), 3);
        EXPECT_THAT(result, UnorderedElementsAre(
            Domain::OrderLine("order1", "RED-CHAIR", 12),
            Domain::OrderLine("order1", "RED-TABLE", 13),
            Domain::OrderLine("order2", "BLUE-LIPSTICK", 14)
        ));
    }

    TEST(Database, test_orderline_mapper_can_save_lines)
    {
        Poco::Data::SQLite::Connector::registerConnector();

        Poco::Data::Session session("SQLite", ":memory:");
        Adapters::Database::InitDatabase(session);

        Domain::OrderLine newLine("order1", "DECORATIVE-WIDGET", 12);
        Adapters::Database::Mapper::OrderLineMapper(session).Insert(newLine);

        std::string orderid; 
        std::string sku;
        size_t qty;

        session << R"(SELECT orderid, sku, qty FROM order_lines)", 
                    Poco::Data::Keywords::into(orderid),
                    Poco::Data::Keywords::into(sku),
                    Poco::Data::Keywords::into(qty),
                    Poco::Data::Keywords::now;

        EXPECT_EQ(newLine, Domain::OrderLine(orderid, sku, qty));
    } 
}