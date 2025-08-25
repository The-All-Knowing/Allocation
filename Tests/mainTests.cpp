#include <gtest/gtest.h>

#include "Services/Loggers/FakeLogger.hpp"


class MyTestEnvironment : public testing::Environment
{
public:
    void SetUp() override
    {
        Allocation::Services::Loggers::InitializeLogger(
            std::make_shared<Allocation::Services::Loggers::FakeLogger>());
    }
};

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    testing::AddGlobalTestEnvironment(new MyTestEnvironment);

    return RUN_ALL_TESTS();
}