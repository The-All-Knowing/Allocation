#include "Handlers/NotFoundHandler.h"

#include "Adapters/Database/Include/Session/SessionPool.h"


namespace Allocation::Infrastructure::Server
{
    void NotFoundHandler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
    {
        response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
        response.send() << "404 Not Found";
    }
}