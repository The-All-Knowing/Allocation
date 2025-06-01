#include "Forwards.h"
#include "Precompile.h"
#include "CommonFunctions.h"
#include "Infrastructure/Server/Include/Server.h"
#include "Adapters/Database/Include/DbTables.h"

#include <gtest/gtest.h>


namespace Allocation::Tests
{
    using namespace Poco;
    using namespace Poco::Net;
    using namespace Poco::JSON;

    TEST(ApiTests, HappyPathReturns201AndAllocatedBatch)
    {
        std::string sku = RandomSku();
        std::string otherSku = RandomSku("other");
        std::string earlyBatch = RandomBatchRef("1");
        std::string laterBatch = RandomBatchRef("2");
        std::string otherBatch = RandomBatchRef("3");

        Poco::Data::SQLite::Connector::registerConnector();
        Poco::Data::Session DBsession("SQLite", "TestBd");
        Adapters::Database::InitDatabase(DBsession);
        InsertBatch(DBsession, earlyBatch, sku, 3);
        InsertBatch(DBsession, laterBatch, sku, 3);
        InsertBatch(DBsession, otherBatch);

        Object::Ptr obj = new Object;
        obj->set("orderid", RandomOrderId());
        obj->set("sku", sku);
        obj->set("qty", 3);

        std::stringstream body;
        obj->stringify(body);

        URI uri("http://127.0.0.1:9999/allocate");
        HTTPClientSession session(uri.getHost(), uri.getPort());
        HTTPRequest request(HTTPRequest::HTTP_POST, uri.getPath(), HTTPMessage::HTTP_1_1);
        request.setContentType("application/json");
        request.setContentLength((int)body.str().length());

        std::ostream& os = session.sendRequest(request);
        os << body.str();

        HTTPResponse response;
        std::istream& rs = session.receiveResponse(response);

        std::stringstream result;
        StreamCopier::copyStream(rs, result);

        auto status = response.getStatus(); 
        EXPECT_EQ(status, HTTPResponse::HTTP_CREATED);

        Parser parser;
        Poco::Dynamic::Var parsed = parser.parse(result);
        Object::Ptr json = parsed.extract<Object::Ptr>();
        std::string batchRef = json->getValue<std::string>("batchref");

        EXPECT_EQ(batchRef, earlyBatch);

        DBsession << "DELETE FROM batches WHERE reference IN (?, ?, ?)",
            Poco::Data::Keywords::use(earlyBatch),
            Poco::Data::Keywords::use(laterBatch),
            Poco::Data::Keywords::use(otherBatch),
            Poco::Data::Keywords::now;
    }

    TEST(ApiTests, UnhappyPathReturns400AndErrorMessage)
    {
        std::string unknown_sku = RandomSku();
        std::string orderid = RandomOrderId();

        Object::Ptr obj = new Object;
        obj->set("orderid", orderid);
        obj->set("sku", unknown_sku);
        obj->set("qty", 20);

        std::stringstream body;
        obj->stringify(body);

        URI uri("http://127.0.0.1:9999/allocate");
        HTTPClientSession session(uri.getHost(), uri.getPort());
        HTTPRequest request(HTTPRequest::HTTP_POST, uri.getPath(), HTTPMessage::HTTP_1_1);
        request.setContentType("application/json");
        request.setContentLength((int)body.str().length());

        std::ostream& os = session.sendRequest(request);
        os << body.str();

        HTTPResponse response;
        std::istream& rs = session.receiveResponse(response);

        std::stringstream result;
        StreamCopier::copyStream(rs, result);

        EXPECT_EQ(response.getStatus(), HTTPResponse::HTTP_BAD_REQUEST);

        Parser parser;
        Poco::Dynamic::Var parsed = parser.parse(result);
        Object::Ptr json = parsed.extract<Object::Ptr>();

        std::string message = json->getValue<std::string>("message");
        EXPECT_EQ(message, "Invalid sku " + unknown_sku);
    }

}