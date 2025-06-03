#pragma once

#include "Precompile.h"


namespace Allocation::Infrastructure::Server
{

    class NotFoundHandler : public Poco::Net::HTTPRequestHandler
    {
    public:
        void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) override;
    };

}