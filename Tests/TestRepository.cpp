#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "Forwards.h"
#include "Precompile.h"
#include "Utilities/Common.h"
#include "CommonFunctions.h"
#include "DbTables.h"
#include "SqlLiteRepository.h"


namespace Allocation::Tests
{
    using ::testing::UnorderedElementsAre;

    TEST(Repository, test_repository_can_save_a_batch)
    {
        Poco::Data::SQLite::Connector::registerConnector();
        Poco::Data::Session session("SQLite", ":memory:");
        Adapters::Database::InitDatabase(session);

        Domain::Batch batch("batch1", "RUSTY-SOAPDISH", 100);
        IRepositoryPtr repo = std::make_shared<Adapters::Repository::SqlLiteRepository>(session);
        repo->Add(batch);

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

    TEST(Repository, test_repository_can_retrieve_a_batch_with_allocations)
    {
        Poco::Data::SQLite::Connector::registerConnector();
        Poco::Data::Session session("SQLite", ":memory:");
        Adapters::Database::InitDatabase(session);   

        int orderlineId = InsertOrderLine(session);
        int batch1Id = InsertBatch(session, "batch1");
        InsertBatch(session, "batch2");
        InsertAllocation(session, orderlineId, batch1Id);

        IRepositoryPtr repo = std::make_shared<Adapters::Repository::SqlLiteRepository>(session);
        auto retrieved = repo->Get("batch1");

        Domain::Batch expected("batch1", "GENERIC-SOFA", 100);
        Domain::OrderLine expectedOrderLine("order1", "GENERIC-SOFA", 12);
        expected.Allocate(expectedOrderLine);

        EXPECT_EQ(retrieved, expected);
        EXPECT_EQ(retrieved->GetSKU(), expected.GetSKU());
        EXPECT_EQ(retrieved->GetAvailableQuantity(), expected.GetAvailableQuantity());
        EXPECT_EQ(retrieved->GetAllocations(), expected.GetAllocations());
    }


    TEST(Repository, test_updating_a_batch)
    {
        Poco::Data::SQLite::Connector::registerConnector();
        Poco::Data::Session session("SQLite", ":memory:");
        Adapters::Database::InitDatabase(session);

        Domain::OrderLine order1("order1", "WEATHERED-BENCH", 10);
        Domain::OrderLine order2("order2", "WEATHERED-BENCH", 20);
        Domain::Batch batch("batch1", "WEATHERED-BENCH", 100);
        batch.Allocate(order1);

        IRepositoryPtr repo = std::make_shared<Adapters::Repository::SqlLiteRepository>(session);
        repo->Add(batch);
        EXPECT_THAT(GetAllocations(session, "batch1"), UnorderedElementsAre("order1"));

        batch.Allocate(order2);
        repo->Add(batch);
        EXPECT_THAT(GetAllocations(session, "batch1"), UnorderedElementsAre("order1", "order2"));
    }

}