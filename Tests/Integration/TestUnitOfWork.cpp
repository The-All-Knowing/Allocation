#include <gtest/gtest.h>

#include "Forwards.h"
#include "Adapters/Database/Session/SessionPool.h"
#include "Adapters/Database/DbTables.h"
#include "Services/UoW/SqlUnitOfWork.h"
#include "CommonFunctions.h"


namespace Allocation::Tests
{

    class UnitOfWorkFixture : public ::testing::Test
    {
    protected:
        std::optional<Poco::Data::Session> session;

        void SetUp() override
        {
            Poco::Data::SQLite::Connector::registerConnector();
            /// @todo: убрать sqlLite, использовать PostgresSQL
            Adapters::Database::SessionPool::Instance().Configure("SQLite", "file:memdb1?mode=memory&cache=shared");
            session = Adapters::Database::SessionPool::Instance().GetSession();
            Adapters::Database::InitDatabase(*session);
        }
    };

    TEST_F(UnitOfWorkFixture, CanRetrieveABatchAndAllocateToIt)
    {
        using namespace Services::UoW;

        const std::string batchRef = "batch1";
        const std::string sku = "HIPSTER-WORKBENCH";
        const std::string orderId = "o1";
        const int qty = 100;
        const int orderQty = 10;

        InsertBatch(*session, batchRef, sku, qty);

        Services::UoW::SqlUnitOfWork uow;
        auto& repo = uow.GetBatchRepository();
        auto batchOpt = repo.Get(batchRef);
        
        ASSERT_TRUE(batchOpt.has_value()) << "Batch should exist in repository";

        auto& batch = batchOpt.value();
        Domain::OrderLine line(orderId, sku, orderQty);
        batch.Allocate(line);

        repo.Add(batch);
        uow.Commit();

        std::string allocatedRef = GetAllocatedBatchRef(*session, orderId, sku);
        EXPECT_EQ(allocatedRef, batchRef);
    }


    TEST_F(UnitOfWorkFixture, RollsBackUncommittedWorkByDefault)
    {
        {
            Services::UoW::SqlUnitOfWork uow;
            auto& repo = uow.GetBatchRepository();

            Domain::Batch batch("batch1", "MEDIUM-PLINTH", 100, std::nullopt);
            repo.Add(batch);
        }

        Poco::Data::Statement select(*session);
        int count = 0;
        select << "SELECT COUNT(*) FROM batches WHERE reference = 'batch1'",
            Poco::Data::Keywords::into(count),
            Poco::Data::Keywords::now;

        EXPECT_EQ(count, 0);
    }

    TEST_F(UnitOfWorkFixture, RollsBackOnError)
    {
        try
        {
            Services::UoW::SqlUnitOfWork uow;
            Domain::IRepository& repo = uow.GetBatchRepository();
            Domain::Batch batch("batch1", "LARGE-FORK", 100);
            repo.Add(batch);

            throw std::exception();
            uow.Commit();
        }
        catch (const std::exception&)
        {}

        Poco::Data::Statement select(*session);
        std::string ref;
        select << "SELECT reference FROM batches",
            Poco::Data::Keywords::into(ref),
            Poco::Data::Keywords::range(0, 1);
        select.execute();

        EXPECT_TRUE(select.done());
    }

}