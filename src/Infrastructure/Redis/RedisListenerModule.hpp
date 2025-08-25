#pragma once

#include "Precompile.hpp"


namespace Allocation::Infrastructure::Redis
{
    class RedisListenerModule : public Poco::Util::Subsystem
    {
    public:
        RedisListenerModule();

        void initialize(Poco::Util::Application& app) override;
        void uninitialize() override;
        const char* name() const override { return "RedisListenerModule"; }

    private:
        void onRedisMessage(const void* sender, Poco::Redis::RedisEventArgs& args);
        void subscribe();

        Poco::Redis::Client _client;
        std::unique_ptr<Poco::Redis::AsyncReader> _reader;
    };
}