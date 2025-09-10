#include <gtest/gtest.h>

#include "Precompile.hpp"

#include "Adapters/Database/Session/SessionPool.hpp"
#include "Adapters/Repository/SqlRepository.hpp"
#include "Tests/Utilities/DatabaseFixture_test.hpp"
#include "Utilities/Common.hpp"


namespace Allocation::Tests
{
    TEST_F(Database_Fixture, test_get_by_batchref)
    {
        try
        {
            Adapters::Repository::SqlRepository repo(_session);
            Domain::Batch b1("b1", "sku1", 100);
            Domain::Batch b2("b2", "sku1", 100);
            Domain::Batch b3("b3", "sku2", 100);
            auto p1 = std::make_shared<Domain::Product>("sku1", std::vector<Domain::Batch>{b1, b2});
            auto p2 = std::make_shared<Domain::Product>("sku2", std::vector<Domain::Batch>{b3});
            repo.Add(p1);
            repo.Add(p2);
            EXPECT_EQ(repo.GetByBatchRef("b2"), p1);
            EXPECT_EQ(repo.GetByBatchRef("b3"), p2);
        }
        catch (const Poco::Exception& e)
        {
            std::cerr << e.displayText(); 
        }
    }
}