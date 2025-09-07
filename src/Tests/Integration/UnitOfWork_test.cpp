#include <gtest/gtest.h>

#include <gmock/gmock.h>

#include "Precompile.hpp"

#include "Adapters/Database/Mappers/ProductMapper.hpp"
#include "Adapters/Database/Session/SessionPool.hpp"
#include "ServiceLayer/UoW/SqlUnitOfWork.hpp"
#include "Tests/Utilities/Common_test.hpp"
#include "Tests/Utilities/DatabaseFixture_test.hpp"
#include "Utilities/Common.hpp"


namespace Allocation::Tests
{
    using namespace Poco::Data::Keywords;

    TEST_F(UoW_Fixture, test_uow_can_retrieve_a_batch_and_allocate_to_it)
    {
        {
            auto session = Adapters::Database::SessionPool::Instance().GetSession();
            Adapters::Database::Mapper::ProductMapper productMapper(session);
            auto product = std::make_shared<Domain::Product>("HIPSTER-WORKBENCH",
                std::vector<Domain::Batch>{{"batch1", "HIPSTER-WORKBENCH", 100}});
            productMapper.Insert(product);
            session.commit();
        }

        ServiceLayer::UoW::SqlUnitOfWork uow;
        auto& productRepo = uow.GetProductRepository();
        auto product = productRepo.Get("HIPSTER-WORKBENCH");

        Domain::OrderLine orderLine("o1", "HIPSTER-WORKBENCH", 10);
        product->Allocate(orderLine);
        uow.Commit();

        {
            auto session = uow.GetSession().value();
            Adapters::Database::Mapper::BatchMapper batchMapper(session);
            auto retrievedBatches = batchMapper.Find("HIPSTER-WORKBENCH");

            EXPECT_EQ(retrievedBatches.size(), 1);
            EXPECT_EQ(retrievedBatches.at(0).GetReference(), "batch1");

            int allocationCount = 0;
            session << "SELECT COUNT(*) FROM allocation.allocations", into(allocationCount), now;

            EXPECT_EQ(allocationCount, 1);
        }

        CleanupForSku("HIPSTER-WORKBENCH");
    }

    TEST_F(UoW_Fixture, test_rolls_back_uncommitted_work_by_default)
    {
        {
            ServiceLayer::UoW::SqlUnitOfWork uow;
            auto session = uow.GetSession().value();
            Adapters::Database::Mapper::ProductMapper productMapper(session);
            auto product = std::make_shared<Domain::Product>(
                "MEDIUM-PLINTH", std::vector<Domain::Batch>{{"batch1", "MEDIUM-PLINTH", 100}});
            productMapper.Insert(product);
        }

        auto session = Adapters::Database::SessionPool::Instance().GetSession();
        int count = 0;
        session << "SELECT COUNT(*) FROM allocation.batches", into(count), now;

        EXPECT_EQ(count, 0);
    }

    TEST_F(UoW_Fixture, test_rolls_back_on_error)
    {
        try
        {
            ServiceLayer::UoW::SqlUnitOfWork uow;
            auto session = uow.GetSession().value();
            Adapters::Database::Mapper::ProductMapper productMapper(session);
            auto product = std::make_shared<Domain::Product>(
                "MEDIUM-PLINTH", std::vector<Domain::Batch>{{"batch1", "MEDIUM-PLINTH", 100}});
            productMapper.Insert(product);

            throw std::runtime_error("boom");
        }
        catch (const std::exception&)
        {
        }

        auto session = Adapters::Database::SessionPool::Instance().GetSession();
        int count = 0;
        session << "SELECT COUNT(*) FROM allocation.batches", into(count), now;

        EXPECT_EQ(count, 0);
    }

    void TryToAllocate(std::string orderid, std::string sku, std::mutex& mutex,
        std::vector<std::string>& exceptions, std::barrier<>& syncPoint)
    {
        try
        {
            Domain::OrderLine line(orderid, sku, 10);
            ServiceLayer::UoW::SqlUnitOfWork uow;
            auto product = uow.GetProductRepository().Get(sku);

            syncPoint.arrive_and_wait();
            product->Allocate(line);

            uow.Commit();
        }
        catch (const std::exception& e)
        {
            std::lock_guard<std::mutex> lock(mutex);
            exceptions.push_back(e.what());
        }
    }

    TEST_F(UoW_Fixture, test_concurrent_updates_to_version_are_not_allowed)
    {
        auto session = Adapters::Database::SessionPool::Instance().GetSession();
        Adapters::Database::Mapper::ProductMapper productMapper(session);
        
        std::string SKU = RandomSku();
        std::string batch = RandomBatchRef();
        auto product = std::make_shared<Domain::Product>(
            SKU, std::vector<Domain::Batch>{{batch, SKU, 100}}, 1);
        productMapper.Insert(product);
        session.commit();

        std::string order1 = RandomOrderId("1");
        std::string order2 = RandomOrderId("2");

        std::mutex exceptionsMutex;
        std::vector<std::string> exceptions;

        std::barrier syncPoint(3);
        std::thread t1(TryToAllocate, order1, SKU, std::ref(exceptionsMutex), std::ref(exceptions),
            std::ref(syncPoint));
        std::thread t2(TryToAllocate, order2, SKU, std::ref(exceptionsMutex), std::ref(exceptions),
            std::ref(syncPoint));

        syncPoint.arrive_and_wait();
        t1.join();
        t2.join();

        int version = 0;
        session << "SELECT version_number FROM allocation.products WHERE sku = $1", use(SKU),
            into(version), now;
        EXPECT_EQ(version, 2);

        EXPECT_EQ(exceptions.size(), 1);

        int orderCount = 0;
        session << R"(
            SELECT COUNT(DISTINCT orderid)
            FROM allocation.allocations
            JOIN allocation.order_lines ON allocations.orderline_id = order_lines.id
            JOIN allocation.batches ON allocations.batch_id = batches.id
            WHERE order_lines.sku = $1
        )",
            use(SKU), into(orderCount), now;

        EXPECT_EQ(orderCount, 1);

        CleanupForSku(SKU);
    }
}