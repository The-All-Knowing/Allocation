#include <gtest/gtest.h>

#include "Infrastructure/Server/Server.hpp"
#include "Precompile.hpp"
#include "Utilities/ApiClient.hpp"
#include "Utilities/Common.hpp"
#include "Utilities/CommonFunctions.hpp"


namespace Allocation::Tests
{
    TEST(ApiTests, HappyPathReturns201AndAllocatedBatch)
    {
        std::string sku = RandomSku();
        std::string otherSku = RandomSku("other");
        std::string earlyBatch = RandomBatchRef("1");
        std::string laterBatch = RandomBatchRef("2");
        std::string otherBatch = RandomBatchRef("3");
        std::chrono::year_month_day laterBatchYmd{
            std::chrono::year{2011}, std::chrono::month{1}, std::chrono::day{2}};
        std::chrono::year_month_day earlyBatchYmd{
            std::chrono::year{2011}, std::chrono::month{1}, std::chrono::day{1}};

        ApiClient::PostToAddBatch(laterBatch, sku, 100, laterBatchYmd);
        ApiClient::PostToAddBatch(earlyBatch, sku, 100, earlyBatchYmd);
        ApiClient::PostToAddBatch(otherBatch, otherSku, 100);

        Poco::JSON::Object::Ptr obj = new Poco::JSON::Object;
        obj->set("orderid", RandomOrderId());
        obj->set("sku", sku);
        obj->set("qty", 3);

        std::stringstream body;
        obj->stringify(body);

        Poco::URI uri = ApiClient::GetURI("/allocate");
        Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());
        Poco::Net::HTTPRequest request(
            Poco::Net::HTTPRequest::HTTP_POST, uri.getPath(), Poco::Net::HTTPMessage::HTTP_1_1);
        request.setContentType("application/json");
        request.setContentLength((int)body.str().length());

        std::ostream& os = session.sendRequest(request);
        os << body.str();

        Poco::Net::HTTPResponse response;
        std::istream& rs = session.receiveResponse(response);

        std::stringstream result;
        Poco::StreamCopier::copyStream(rs, result);

        auto status = response.getStatus();
        EXPECT_EQ(status, Poco::Net::HTTPResponse::HTTP_OK);

        Poco::JSON::Parser parser;
        Poco::Dynamic::Var parsed = parser.parse(result);
        Poco::JSON::Object::Ptr json = parsed.extract<Poco::JSON::Object::Ptr>();
        std::string batchRef = json->getValue<std::string>("batchref");

        EXPECT_EQ(batchRef, earlyBatch);
    }

    TEST(ApiTests, UnhappyPathReturns400AndErrorMessage)
    {
        std::string unknownSku = RandomSku();
        std::string orderId = RandomOrderId();

        Poco::JSON::Object::Ptr obj = new Poco::JSON::Object;
        obj->set("orderid", orderId);
        obj->set("sku", unknownSku);
        obj->set("qty", 20);

        std::stringstream body;
        obj->stringify(body);

        Poco::URI uri = ApiClient::GetURI("/allocate");
        Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());
        Poco::Net::HTTPRequest request(
            Poco::Net::HTTPRequest::HTTP_POST, uri.getPath(), Poco::Net::HTTPMessage::HTTP_1_1);
        request.setContentType("application/json");
        request.setContentLength((int)body.str().length());

        std::ostream& os = session.sendRequest(request);
        os << body.str();

        Poco::Net::HTTPResponse response;
        std::istream& rs = session.receiveResponse(response);

        std::stringstream result;
        Poco::StreamCopier::copyStream(rs, result);

        EXPECT_EQ(response.getStatus(), Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);

        Poco::JSON::Parser parser;
        Poco::Dynamic::Var parsed = parser.parse(result);
        Poco::JSON::Object::Ptr json = parsed.extract<Poco::JSON::Object::Ptr>();

        std::string message = json->getValue<std::string>("message");
        EXPECT_EQ(message, "Invalid sku " + unknownSku);
    }
}