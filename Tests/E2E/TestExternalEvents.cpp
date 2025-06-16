#include <gtest/gtest.h>

#include "Precompile.h"
#include "CommonFunctions.h"
#include "Infrastructure/Redis/RedisConfig.h"
#include "Utilities/Common.h"
#include "RedisClient.h"
#include "ApiClient.h"


namespace Allocation::Tests
{
    TEST(ExternalEvents, ChangeBatchQuantityLeadsToReallocation)
    {
        auto conf = Infrastructure::Redis::RedisConfig::FromConfig();

        std::string orderid = RandomOrderId();
        std::string sku = RandomSku();
        std::string earlierBatch = RandomBatchRef("old");
        std::string laterBatch = RandomBatchRef("newer");

        using namespace std::chrono;
        const year_month_day today(2011y, December, 1d);
        const auto tomorrow = today + days(1);

        ApiClient::PostToAddBatch(earlierBatch, sku, 10, today);
        ApiClient::PostToAddBatch(laterBatch, sku, 10, tomorrow);
        auto response = ApiClient::PostToAllocate(orderid, sku, 10);
        EXPECT_EQ(response, earlierBatch);

        RedisClient subscriber(conf->host, conf->port, "line_allocated");

        Poco::Redis::Client redis(conf->host, conf->port);
        {
            Poco::JSON::Object obj;
            obj.set("batchref", earlierBatch);
            obj.set("qty", 5);

            std::stringstream ss;
            obj.stringify(ss);

            Poco::Redis::Command publish("PUBLISH");
            publish.add("change_batch_quantity");
            publish.add(ss.str());

            redis.execute<void>(publish);
        }

        bool got = subscriber.WaitForMessage(3000);
        EXPECT_TRUE(got) << "Redis event was not received in time";

        auto msgOpt = subscriber.GetMessage();
        EXPECT_TRUE(msgOpt.has_value());

        Poco::JSON::Parser parser;
        auto result = parser.parse(msgOpt.value());
        auto obj = result.extract<Poco::JSON::Object::Ptr>();

        EXPECT_EQ(obj->getValue<std::string>("orderid"), orderid);
        EXPECT_EQ(obj->getValue<std::string>("batchref"), laterBatch);
    }
}