#include <gtest/gtest.h>

#include <gmock/gmock.h>

#include "Precompile.hpp"

#include "Adapters/Database/Mappers/ProductMapper.hpp"
#include "Adapters/Database/Session/SessionPool.hpp"
#include "Services/UoW/SqlUnitOfWork.hpp"
#include "Tests/Utilities/Common_test.hpp"
#include "Tests/Utilities/InitDatabase_test.hpp"
#include "Utilities/Common.hpp"


namespace Allocation::Tests
{
    using UoW = InitDatabase;

    TEST(UoW, test_uow_can_retrieve_a_batch_and_allocate_to_it)
    {
        auto session = Adapters::Database::SessionPool::Instance().GetSession();

        Adapters::Database::Mapper::ProductMapper productMapper(session);
        auto insertProduct = std::make_shared<Domain::Product>(
            "HIPSTER-WORKBENCH", std::vector<Domain::Batch>{{"batch1", "HIPSTER-WORKBENCH", 100}});
        productMapper.Insert(insertProduct);

        try
        {
            Services::UoW::SqlUnitOfWork uow;
            auto& productRepo = uow.GetProductRepository();
            auto product = productRepo.Get("HIPSTER-WORKBENCH");
            Domain::OrderLine orderLine("o1", "HIPSTER-WORKBENCH", 10);
            product->Allocate(orderLine);
            uow.Commit();

            Adapters::Database::Mapper::BatchMapper batchMapper(session);
            auto retrievedBatches = batchMapper.Find("HIPSTER-WORKBENCH");
            EXPECT_EQ(retrievedBatches.size(), 1);
            EXPECT_EQ(retrievedBatches.at(0).GetReference(), "batch1");
        }
        catch (const Poco::Exception& e)
        {
            FAIL() << e.displayText();
        }

        // Очистка данных
        session << "DELETE FROM allocation.products WHERE sku = 'HIPSTER-WORKBENCH'",
            Poco::Data::Keywords::now;
    }
}
/*
    TEST(UoW, test_rolls_back_uncommitted_work_by_default)
    {
        try
        {
            {
                Services::UoW::SqlUnitOfWork uow;
                InsertBatch(uow.GetSession().value(), "batch1", "MEDIUM-PLINTH-TEST", 100);
            }

            auto session = Adapters::Database::SessionPool::Instance().GetSession();
            int count = 0;
            session << "SELECT COUNT(*) FROM public.batches", Poco::Data::Keywords::into(count),
                Poco::Data::Keywords::now;

            EXPECT_EQ(count, 0);
        }
        catch (const Poco::Exception& e)
        {
            FAIL() << e.displayText();
        }
    }

    TEST(UoW, test_rolls_back_on_error)
    {
        try
        {
            Services::UoW::SqlUnitOfWork uow;
            InsertBatch(uow.GetSession().value(), "batch1", "LARGE-FORK", 100);

            throw std::exception();
        }
        catch (const std::exception&)
        {
        }

        auto session = Adapters::Database::SessionPool::Instance().GetSession();
        int count = 0;
        session << "SELECT COUNT(*) FROM public.batches", Poco::Data::Keywords::into(count),
            Poco::Data::Keywords::now;

        EXPECT_EQ(count, 0);
    }

    void TryToAllocate(std::string orderid, std::string sku, std::mutex& mutex,
        std::vector<std::string>& exceptions)
    {
        try
        {
            Domain::OrderLine line(orderid, sku, 10);
            Services::UoW::SqlUnitOfWork uow;
            auto product = uow.GetProductRepository().Get(sku);
            product->Allocate(line);
            uow.GetProductRepository().Add(product);
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            uow.Commit();
        }
        catch (const std::exception& e)
        {
            std::lock_guard<std::mutex> lock(mutex);
            exceptions.push_back(e.what());
        }
    }

    TEST(UoW, test_concurrent_updates_to_version_are_not_allowed)
    {
        auto session = Adapters::Database::SessionPool::Instance().GetSession();
        std::string SKU = RandomSku();
        std::string batch = RandomBatchRef();
        InsertBatch(session, batch, SKU, 100, 1);
        session.commit();
        try
        {
            std::string order1 = RandomOrderId("1");
            std::string order2 = RandomOrderId("2");

            std::mutex exceptions_mutex;
            std::vector<std::string> exceptions;

            std::thread t1(
                TryToAllocate, order1, SKU, std::ref(exceptions_mutex), std::ref(exceptions));
            std::thread t2(
                TryToAllocate, order2, SKU, std::ref(exceptions_mutex), std::ref(exceptions));

            t1.join();
            t2.join();

            int version = 0;
            session << "SELECT version_number FROM public.products WHERE sku = $1",
                Poco::Data::Keywords::use(SKU), Poco::Data::Keywords::into(version),
                Poco::Data::Keywords::now;
            EXPECT_EQ(version, 2);

            EXPECT_EQ(exceptions.size(), 1);

            int order_count = 0;
            session << R"(
                SELECT COUNT(DISTINCT orderid)
                FROM public.allocations
                JOIN public.order_lines ON allocations.orderline_id = order_lines.id
                JOIN public.batches ON allocations.batch_id = batches.id
                WHERE order_lines.sku = $1
            )",
                Poco::Data::Keywords::use(SKU), Poco::Data::Keywords::into(order_count),
                Poco::Data::Keywords::now;
            EXPECT_EQ(order_count, 1);
        }
        catch (const Poco::Exception& e)
        {
            FAIL() << e.displayText();
        }

        DeleteProduct(session, SKU);
    }
}
    */