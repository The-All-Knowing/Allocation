#include "HandlerFactory.hpp"

#include "Handlers/AddBatchHandler.hpp"
#include "Handlers/AllocateHandler.hpp"
#include "Handlers/AllocationsViewHandler.hpp"
#include "Handlers/NotFoundHandler.hpp"
#include "Utilities/Loggers/ILogger.hpp"


namespace Allocation::Entrypoints::Rest
{
    Poco::Net::HTTPRequestHandler* HandlerFactory::createRequestHandler(
        const Poco::Net::HTTPServerRequest& request)
    {
        Allocation::Loggers::GetLogger()->Information(
            std::format("Request: {} {}", request.getMethod(), request.getURI()));

        if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_POST)
        {
            if (request.getURI() == "/allocate")
                return new Handlers::AllocateHandler;

            if (request.getURI() == "/add_batch")
                return new Handlers::AddBatchHandler;
        }

        if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_GET)
        {
            if (request.getURI().find("/allocations/") == 0)
                return new Handlers::AllocationsViewHandler;
        }

        return new Handlers::NotFoundHandler;
    }
}
