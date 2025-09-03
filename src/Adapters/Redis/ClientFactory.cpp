#include "ClientFactory.hpp"


namespace Allocation::Adapters::Redis
{
    ClientFactory& ClientFactory::Instance() noexcept
    {
        static ClientFactory instance;
        return instance;
    }

    bool ClientFactory::IsConfigured() const noexcept { return !_address.toString().empty(); }

    void ClientFactory::Configure(const Poco::Net::SocketAddress& address) noexcept
    {
        _address = address;
    }

    Poco::Redis::Client::Ptr ClientFactory::Create()
    {
        if (_address.toString().empty())
            throw std::runtime_error("ClientFactory is not configured");

        return new Poco::Redis::Client(_address);
    }
}