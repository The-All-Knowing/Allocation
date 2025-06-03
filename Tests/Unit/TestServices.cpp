#include <gtest/gtest.h>

#include "Forwards.h"
#include "CommonFunctions.h"
#include "Services.h"
#include "InvalidSku.h"
#include "Adapters/Repository/FakeRepository.h"
#include "Adapters/Database/Session/FakeSessionImpl.h"


namespace Allocation::Tests
{

    TEST(Services, test_returns_allocation)
    {
        Domain::OrderLine line("o1", "COMPLICATED-LAMP", 10);
        Domain::Batch batch("b1", "COMPLICATED-LAMP", 100);
        
        IRepositoryPtr repo = std::make_shared<Adapters::Repository::FakeRepository>(
            std::vector<Domain::Batch>{ batch });
        Poco::Data::Session session(new Adapters::Database::FakeSessionImpl());

        auto result = Services::Allocate(line, repo, session);
        EXPECT_EQ(result, "b1");
    }

    TEST(Services, throws_for_invalid_sku)
    {
        Domain::OrderLine line("o1", "NONEXISTENTSKU", 10);
        Domain::Batch batch("b1", "AREALSKU", 100);

        auto repo = std::make_shared<Adapters::Repository::FakeRepository>(
            std::vector<Domain::Batch>{ batch });
        Poco::Data::Session session(new Adapters::Database::FakeSessionImpl());

        EXPECT_TRUE(
        ThrowsWithMessage<Services::InvalidSku>(
            [&]() {Services::Allocate(line, repo, session);},
            "Invalid sku NONEXISTENTSKU")
        );
    }

    TEST(Services, test_commits)
    {
        Domain::OrderLine line("o1", "OMINOUS-MIRROR", 10);
        Domain::Batch batch("b1", "OMINOUS-MIRROR", 100);
        auto repo = std::make_shared<Adapters::Repository::FakeRepository>(
            std::vector<Domain::Batch>{ batch });
        Poco::Data::Session session(new Adapters::Database::FakeSessionImpl());

        Services::Allocate(line, repo, session);

        EXPECT_FALSE(session.isTransaction());
    }

}
