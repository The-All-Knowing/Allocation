#include <gtest/gtest.h>

#include "Tests/Utilities/FakeLogger_test.hpp"


class TestEnvironment : public testing::Environment
{
public:
    void SetUp() override
    {
        Allocation::Loggers::InitializeLogger(
            std::make_shared<Allocation::Tests::FakeLogger>());
    }
};

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    testing::AddGlobalTestEnvironment(new TestEnvironment);

    return RUN_ALL_TESTS();
}