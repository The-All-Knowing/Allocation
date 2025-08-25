#include "RedisConfig.hpp"


namespace Allocation::Infrastructure::Redis
{
    std::shared_ptr<RedisConfig> RedisConfig::FromConfig()
    {
        static auto result = []
        {
            Poco::Path exePath(Poco::Path::current());
            exePath.append("Allocation.ini");

            Poco::AutoPtr<Poco::Util::IniFileConfiguration> pConf(
                new Poco::Util::IniFileConfiguration(exePath.toString()));

            std::string host = pConf->getString("redis.host", "127.0.0.1");
            int port = pConf->getInt("redis.port", 6379);
            return std::make_shared<RedisConfig>(host, port);
        }();
        return result;
    }
}