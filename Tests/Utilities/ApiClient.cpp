#include "ApiClient.h"
#include <gtest/gtest.h>
#include "Utilities/Common.h"



namespace Allocation::Tests::ApiClient
{
    Poco::URI GetURI(const std::string& command)
    {
        Poco::AutoPtr<Poco::Util::IniFileConfiguration> pConf(
        new Poco::Util::IniFileConfiguration("./Allocation.ini"));
        
        std::string host = pConf->getString("server.host", "127.0.0.1");
        int port = pConf->getInt("server.port", 9980);

        Poco::URI uri;
        uri.setScheme("http");
        uri.setHost(host);
        uri.setPort(port);
        uri.setPath(command);

        return uri;
    }

    void PostToAddBatch(const std::string& ref, const std::string& sku, int qty,
        std::optional<std::chrono::year_month_day> eta)
    {
        Poco::JSON::Object::Ptr obj = new Poco::JSON::Object;
        obj->set("ref", ref);
        obj->set("sku", sku);
        obj->set("qty", qty);
        if (eta.has_value())
            obj->set("eta", Convert(eta.value()));

        std::stringstream body;
        obj->stringify(body);

        Poco::URI uri = GetURI("/add_batch");
        Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());
        Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, uri.getPath(), Poco::Net::HTTPMessage::HTTP_1_1);
        request.setContentType("application/json");
        request.setContentLength(static_cast<int>(body.str().length()));

        std::ostream& os = session.sendRequest(request);
        os << body.str();

        Poco::Net::HTTPResponse response;
        session.receiveResponse(response);

        EXPECT_EQ(response.getStatus(), Poco::Net::HTTPResponse::HTTP_CREATED);
    }

    std::string PostToAllocate(
        const std::string& orderid,
        const std::string& sku,
        int qty,
        bool expectSuccess)
    {
        Poco::URI uri = GetURI("/allocate");
        Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());

        Poco::JSON::Object::Ptr json = new Poco::JSON::Object;
        json->set("orderid", orderid);
        json->set("sku", sku);
        json->set("qty", qty);

        std::stringstream requestBody;
        json->stringify(requestBody);

        Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, uri.getPath(), "HTTP/1.1");
        request.setContentType("application/json");
        request.setContentLength(requestBody.str().length());

        std::ostream& os = session.sendRequest(request);
        os << requestBody.str();

        Poco::Net::HTTPResponse response;
        std::istream& rs = session.receiveResponse(response);

        if (expectSuccess)
        {
            EXPECT_EQ(response.getStatus(), Poco::Net::HTTPResponse::HTTP_OK);
        }

        std::stringstream result;
        Poco::StreamCopier::copyStream(rs, result);

        Poco::JSON::Parser parser;
        Poco::Dynamic::Var parsed = parser.parse(result);
        Poco::JSON::Object::Ptr requestJson = parsed.extract<Poco::JSON::Object::Ptr>();
        std::string batchRef = requestJson->getValue<std::string>("batchref");

        return batchRef;
    }
}