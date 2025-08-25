#pragma once

#include "Domain/Events/Allocated.hpp"
#include "Precompile.hpp"



namespace Allocation::Adapters::Redis
{
    class RedisEventPublisher
    {
    public:
        RedisEventPublisher(const std::string& host, int port);

        void Publish(const std::string& channel, std::shared_ptr<Domain::Events::Allocated> event);

    private:
        Poco::Redis::Client _client;
    };
}