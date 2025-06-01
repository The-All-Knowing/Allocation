#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "Precompile.h"
#include "CommonFunctions.h"
#include "Utilities/Common.h"
#include "Adapters/Database/Include/DbTables.h"
#include "Adapters/Database/Include/Mappers/OrderLineMapper.h"
#include "Adapters/Database/Include/Mappers/BatchMapper.h"


namespace Allocation::Tests
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

    TEST(Database, test_retrieving_batches)
    {
        Poco::Data::SQLite::Connector::registerConnector();
        Poco::Data::Session session("SQLite", ":memory:");
        Adapters::Database::InitDatabase(session);

        session << R"(INSERT INTO batches (reference, sku, _purchased_quantity, eta) 
                    VALUES ("batch1", "sku1", 100, null))",
                Poco::Data::Keywords::now;

        session << R"(INSERT INTO batches (reference, sku, _purchased_quantity, eta)
                    VALUES ("batch2", "sku2", 200, "2011-04-11"))",
                Poco::Data::Keywords::now;
        
        std::chrono::year_month_day eta(std::chrono::year{2011}, std::chrono::month{4}, std::chrono::day{11});
        EXPECT_THAT(
            Adapters::Database::Mapper::BatchMapper(session).GetAll(),
            ::testing::UnorderedElementsAre(
                Domain::Batch("batch1", "sku1", 100),
                Domain::Batch("batch2", "sku2", 200, eta)));
    }

    TEST(Database, test_saving_batches)
    {
        Poco::Data::SQLite::Connector::registerConnector();
        Poco::Data::Session session("SQLite", ":memory:");
        Adapters::Database::InitDatabase(session);

        Domain::Batch batch("batch1", "sku1", 100);
        Adapters::Database::Mapper::BatchMapper(session).Insert(batch);

        std::string reference; 
        std::string sku;
        size_t purchasedQuantity;
        Poco::Nullable<Poco::DateTime> eta; 

        session << R"(SELECT reference, sku, _purchased_quantity, eta FROM batches)", 
                    Poco::Data::Keywords::into(reference),
                    Poco::Data::Keywords::into(sku),
                    Poco::Data::Keywords::into(purchasedQuantity),
                    Poco::Data::Keywords::into(eta),
                    Poco::Data::Keywords::now;

        EXPECT_EQ(batch.GetReference(), reference);
        EXPECT_EQ(batch.GetSKU(), sku);
        EXPECT_EQ(batch.GetAvailableQuantity(), purchasedQuantity);
        EXPECT_TRUE(eta.isNull());
    }

    TEST(Database,  test_saving_allocations)
    {
        Poco::Data::SQLite::Connector::registerConnector();
        Poco::Data::Session session("SQLite", ":memory:");
        Adapters::Database::InitDatabase(session);

        Domain::Batch batch("batch1", "sku1", 100);
        Domain::OrderLine line("order1", "sku1", 10);
        batch.Allocate(line);

        Adapters::Database::Mapper::BatchMapper(session).Insert(batch);

        size_t orderlineId;
        size_t batchId;

        session << R"(SELECT orderline_id, batch_id FROM "allocations")",
                    Poco::Data::Keywords::into(orderlineId),
                    Poco::Data::Keywords::into(batchId),
                    Poco::Data::Keywords::now;

        EXPECT_EQ(orderlineId, 1);
        EXPECT_EQ(batchId, 1);
    }

    TEST(Database, test_retrieving_allocations)
    {
        Poco::Data::SQLite::Connector::registerConnector();
        Poco::Data::Session session("SQLite", ":memory:");
        Adapters::Database::InitDatabase(session);

        int orderId = InsertOrderLine(session, "order1", "sku1", 12);
        int batchId = InsertBatch(session, "batch1", "sku1", 100);
        InsertAllocation(session, orderId, batchId);

        auto batch = Adapters::Database::Mapper::BatchMapper(session).FindByReference("batch1");

        EXPECT_EQ(batch->GetReference(), "batch1");
        EXPECT_EQ(batch->GetSKU(), "sku1");

        EXPECT_THAT(
            batch->GetAllocations(),
            UnorderedElementsAre(Domain::OrderLine("order1", "sku1", 12))
        );
    }

}