#include "HandlerFactory.h"
#include "Handlers/AllocateHandler.h"
#include "Handlers/NotFoundHandler.h"
#include "Handlers/AddBatchHandler.h"
#include "Services/Loggers/ILogger.h"


namespace Allocation::Infrastructure::Server
{

    Poco::Net::HTTPRequestHandler* HandlerFactory::createRequestHandler(const Poco::Net::HTTPServerRequest& request)
    {
        Services::Loggers::GetLogger()->Information(std::format("Request: {} {}", request.getMethod(), request.getURI()));

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
