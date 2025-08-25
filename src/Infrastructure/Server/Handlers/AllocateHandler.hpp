#pragma once

#include "Precompile.hpp"


namespace Allocation::Infrastructure::Server::Handlers
{
    class AllocateHandler final : public Poco::Net::HTTPRequestHandler
    {
    public:
        void handleRequest(Poco::Net::HTTPServerRequest& request,
            Poco::Net::HTTPServerResponse& response) override;
    };
}