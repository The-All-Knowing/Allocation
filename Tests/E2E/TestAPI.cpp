#include <gtest/gtest.h>

#include "Forwards.h"
#include "Precompile.h"
#include "CommonFunctions.h"
#include "Utilities/Common.h"
#include "Infrastructure/Server/Server.h"


namespace Allocation::Tests
{
    void PostToAddBatch(const std::string& ref, const std::string& sku, int qty, std::optional<std::chrono::year_month_day> eta = std::nullopt)
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
        std::istream& rs = session.receiveResponse(response);

        ASSERT_EQ(response.getStatus(), Poco::Net::HTTPResponse::HTTP_CREATED);
    }

    TEST(ApiTests, HappyPathReturns201AndAllocatedBatch)
    {
        std::string sku = RandomSku();
        std::string otherSku = RandomSku("other");
        std::string earlyBatch = RandomBatchRef("1");
        std::string laterBatch = RandomBatchRef("2");
        std::string otherBatch = RandomBatchRef("3");
        std::chrono::year_month_day laterBatchYmd{std::chrono::year{2011}, std::chrono::month{1}, std::chrono::day{2}};
        std::chrono::year_month_day earlyBatchYmd{std::chrono::year{2011}, std::chrono::month{1}, std::chrono::day{1}};

        PostToAddBatch(laterBatch, sku, 100, laterBatchYmd);
        PostToAddBatch(earlyBatch, sku, 100, earlyBatchYmd);
        PostToAddBatch(otherBatch, otherSku, 100);

        Poco::JSON::Object::Ptr obj = new Poco::JSON::Object;
        obj->set("orderid", RandomOrderId());
        obj->set("sku", sku);
        obj->set("qty", 3);

        std::stringstream body;
        obj->stringify(body);

        Poco::URI uri = GetURI("/allocate");
        Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());
        Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, uri.getPath(), Poco::Net::HTTPMessage::HTTP_1_1);
        request.setContentType("application/json");
        request.setContentLength((int)body.str().length());

        std::ostream& os = session.sendRequest(request);
        os << body.str();

        Poco::Net::HTTPResponse response;
        std::istream& rs = session.receiveResponse(response);

        std::stringstream result;
        Poco::StreamCopier::copyStream(rs, result);

        auto status = response.getStatus(); 
        EXPECT_EQ(status, Poco::Net::HTTPResponse::HTTP_CREATED);

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

        Poco::URI uri = GetURI("/allocate");
        Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());
        Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, uri.getPath(), Poco::Net::HTTPMessage::HTTP_1_1);
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