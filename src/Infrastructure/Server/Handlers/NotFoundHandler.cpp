#include "NotFoundHandler.hpp"


namespace Allocation::Infrastructure::Server::Handlers
{
    void NotFoundHandler::handleRequest(
        Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
    {
        response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
        response.send() << "404 Not Found";
    }
}