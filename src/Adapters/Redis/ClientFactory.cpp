#include "ClientFactory.hpp"


namespace Allocation::Adapters::Redis
{
    ClientFactory& ClientFactory::Instance() noexcept
    {
        static ClientFactory instance;
        return instance;
    }

    bool ClientFactory::IsConfigured() const noexcept { return !_address.toString().empty(); }

    void ClientFactory::Configure(const RedisConfig& config) noexcept
    { 
        _address = Poco::Net::SocketAddress(config.path, config.port);
    }

    Poco::Redis::Client::Ptr ClientFactory::Create()
    {
        if (_address.toString().empty())
            throw std::runtime_error("ClientFactory is not configured");

        Poco::Redis::Client::Ptr client = new Poco::Redis::Client();
        client->connect(_address);
        return client;
    }
}