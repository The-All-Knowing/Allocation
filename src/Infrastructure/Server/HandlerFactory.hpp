#pragma once

#include "Precompile.hpp"


namespace Allocation::Infrastructure::Server
{
    class HandlerFactory : public Poco::Net::HTTPRequestHandlerFactory
    {
    public:
        Poco::Net::HTTPRequestHandler* createRequestHandler(
            const Poco::Net::HTTPServerRequest& request) override;
    };
}