#include <gtest/gtest.h>

#include "Precompile.h"
#include "Adapters/Database/Session/ConnectionConfig.h"


namespace Allocation::Tests
{

    class UoWFixture : public ::testing::Test
    {
    protected:
        void SetUp() override;
        void TearDown() override;

        static void SetUpTestSuite();
        
    private:
        static Adapters::Database::ConnectionConfig LoadDatabaseConfig(const std::string& path);
    };
}