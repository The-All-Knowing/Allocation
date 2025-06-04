#include "HandlerFactory.h"
#include "Handlers/AllocateHandler.h"
#include "Handlers/NotFoundHandler.h"
#include "Handlers/AddBatchHandler.h"


namespace Allocation::Infrastructure::Server
{

    Poco::Net::HTTPRequestHandler* HandlerFactory::createRequestHandler(const Poco::Net::HTTPServerRequest& request)
    {
        Poco::Util::Application::instance().logger().information("Request: %s %s", request.getMethod(), request.getURI());

        if (request.getMethod() == "POST")
        {
            if (request.getURI() == "/allocate")
                return new Handlers::AllocateHandler;

            if (request.getURI() == "/add_batch")
                return new Handlers::AddBatchHandler;
        }

        return new Handlers::NotFoundHandler;
    }

}        
