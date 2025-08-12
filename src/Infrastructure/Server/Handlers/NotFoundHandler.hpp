#pragma once

#include "Precompile.hpp"


namespace Allocation::Infrastructure::Server::Handlers
{
    class NotFoundHandler final : public Poco::Net::HTTPRequestHandler
    {
    public:
        void handleRequest(Poco::Net::HTTPServerRequest& request,
            Poco::Net::HTTPServerResponse& response) override;
    };
}