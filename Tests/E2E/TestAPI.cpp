#include <gtest/gtest.h>

#include "Forwards.h"
#include "Precompile.h"
#include "CommonFunctions.h"
#include "Infrastructure/Server/Server.h"
#include "Adapters/Database/DbTables.h"


namespace Allocation::Tests
{

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

        Poco::AutoPtr<Poco::Util::IniFileConfiguration> pConf(
            new Poco::Util::IniFileConfiguration("./Allocation.ini"));
        
        std::string host = pConf->getString("server.host", "127.0.0.1");
        int port = pConf->getInt("server.port", 9980);

        Poco::URI uri;
        uri.setScheme("http");
        uri.setHost(host);
        uri.setPort(port);
        uri.setPath("/allocate");

        Poco::JSON::Object::Ptr obj = new Poco::JSON::Object;
        obj->set("orderid", RandomOrderId());
        obj->set("sku", sku);
        obj->set("qty", 3);

        std::stringstream body;
        obj->stringify(body);

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

        DBsession << "DELETE FROM batches WHERE reference IN (?, ?, ?)",
            Poco::Data::Keywords::use(earlyBatch),
            Poco::Data::Keywords::use(laterBatch),
            Poco::Data::Keywords::use(otherBatch),
            Poco::Data::Keywords::now;
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

        Poco::AutoPtr<Poco::Util::IniFileConfiguration> pConf(
        new Poco::Util::IniFileConfiguration("./Allocation.ini"));
        
        std::string host = pConf->getString("server.host", "127.0.0.1");
        int port = pConf->getInt("server.port", 9980);

        Poco::URI uri;
        uri.setScheme("http");
        uri.setHost(host);
        uri.setPort(port);
        uri.setPath("/allocate");

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