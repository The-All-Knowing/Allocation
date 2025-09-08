#pragma once

#include "Precompile.hpp"


namespace Allocation::Entrypoints::Rest::Handlers
{
    class NotFoundHandler final : public Poco::Net::HTTPRequestHandler
    {
    public:
        void handleRequest(Poco::Net::HTTPServerRequest& request,
            Poco::Net::HTTPServerResponse& response) override;
    };
}