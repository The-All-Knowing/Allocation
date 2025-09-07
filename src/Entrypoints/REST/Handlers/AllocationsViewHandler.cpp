#include "AllocationsViewHandler.hpp"

#include "Utilities/Loggers/ILogger.hpp"
#include "ServiceLayer/UoW/SqlUnitOfWork.hpp"
#include "ServiceLayer/Views.hpp"


namespace Allocation::Entrypoints::Rest::Handlers
{
    void AllocationsViewHandler::handleRequest(
        Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
    {
        try
        {
            std::string uri = request.getURI();
            std::string prefix = "/allocations/";
            if (uri.find(prefix) != 0 || uri.size() <= prefix.size())
            {
                response.setStatus(Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
                response.send() << "Invalid URL";
                return;
            }
            std::string orderid = uri.substr(prefix.size());

            ServiceLayer::UoW::SqlUnitOfWork uow;
            auto results = ServiceLayer::Views::Allocations(orderid, uow);
            if (results.empty())
            {
                response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
                response.send() << "not found";
                return;
            }

            Poco::JSON::Array jsonArray;
            for (const auto& [sku, batchref] : results)
            {
                auto obj = new Poco::JSON::Object;
                obj->set("sku", sku);
                obj->set("batchref", batchref);
                jsonArray.add(obj);
            }
            response.setContentType("application/json");
            response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
            std::ostream& out = response.send();
            jsonArray.stringify(out);
        }
        catch (const std::exception& ex)
        {
            response.setStatus(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
            auto msg = ex.what();
            response.send() << "Internal Server Error: " << msg;

            Allocation::Loggers::GetLogger()->Error(msg);
        }
    }
}