#include "AllocationsViewHandler.hpp"

#include "ServiceLayer/UoW/SqlUnitOfWork.hpp"
#include "ServiceLayer/Views.hpp"
#include "Utilities/Loggers/ILogger.hpp"


namespace Allocation::Entrypoints::Rest::Handlers
{
    void AllocationsViewHandler::handleRequest(
        Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
    {
        try
        {
            std::string uri = request.getURI();
            std::string prefix = "/allocations/";
            std::string orderid = uri.substr(prefix.size());
            if (orderid.empty())
                throw std::invalid_argument("Invalid orderId");

            ServiceLayer::UoW::SqlUnitOfWork uow;
            auto results = ServiceLayer::Views::Allocations(orderid, uow);
            if (results.empty())
            {
                response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
                response.setContentType("application/json");
                response.send() << R"({"error":"not found"})";
                return;
            }

            Poco::JSON::Array jsonArray;
            for (const auto& [sku, batchref] : results)
            {
                Poco::JSON::Object::Ptr obj = new Poco::JSON::Object;
                obj->set("sku", sku);
                obj->set("batchref", batchref);
                jsonArray.add(obj);
            }
            response.setContentType("application/json");
            response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
            jsonArray.stringify(response.send());
        }
        catch (const Poco::Exception& ex)
        {
            response.setStatus(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
            response.setContentType("application/json");
            std::string msg = ex.displayText();
            response.send() << "{\"error\":\"" << msg << "\"}";

            Allocation::Loggers::GetLogger()->Error(msg);
        }
        catch (const std::exception& ex)
        {
            response.setStatus(Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
            response.setContentType("application/json");
            std::ostream& ostr = response.send();
            std::string msg = ex.what();
            ostr << "{\"message\": \"" << msg << "\"}";

            Allocation::Loggers::GetLogger()->Error(msg);
        }
    }
}