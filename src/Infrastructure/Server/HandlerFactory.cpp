#include "HandlerFactory.h"
#include "Handlers/AllocateHandler.h"
#include "Handlers/NotFoundHandler.h"


namespace Allocation::Infrastructure::Server
{

    Poco::Net::HTTPRequestHandler* HandlerFactory::createRequestHandler(const Poco::Net::HTTPServerRequest& request)
    {
        Poco::Util::Application::instance().logger().information("Request: %s %s", request.getMethod(), request.getURI());

        if (request.getURI() == "/allocate" && request.getMethod() == "POST")
        {
            return new AllocateHandler;
        }

        return new NotFoundHandler;
    }

}        
