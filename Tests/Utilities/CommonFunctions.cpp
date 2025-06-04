#include "CommonFunctions.h"
#include "Adapters/Database/Session/FakeSessionImpl.h"


namespace Allocation::Tests
{
    Poco::URI GetURI(std::string command)
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

    Poco::Data::Session GetFakeSession()
    {
        return Poco::Data::Session(new Adapters::Database::FakeSessionImpl());
    }

    std::chrono::year_month_day GetCurrentDate()
    {
        const std::chrono::time_point now{std::chrono::system_clock::now()};
        return std::chrono::floor<std::chrono::days>(now);
    }

    std::pair<Domain::Batch, Domain::OrderLine> MakeBatchAndLine(
        const std::string& SKU, size_t batchQty, size_t lineQty)
    {
        return {Domain::Batch("batch-001", SKU, batchQty, GetCurrentDate()),
                Domain::OrderLine("order-123", SKU, lineQty)};
    }

    int InsertOrderLine(Poco::Data::Session& session, std::string orderId, std::string sku, int qty)
    {
        session << R"(INSERT INTO order_lines (orderid, sku, qty) VALUES (?, ?, ?))",
            Poco::Data::Keywords::use(orderId),
            Poco::Data::Keywords::use(sku),
            Poco::Data::Keywords::use(qty),
            Poco::Data::Keywords::now;

        int id = 0;
        session << R"(SELECT id FROM order_lines WHERE orderid = ? AND sku = ?)",
            Poco::Data::Keywords::into(id),
            Poco::Data::Keywords::use(orderId),
            Poco::Data::Keywords::use(sku),
            Poco::Data::Keywords::now;

        return id;
    }

    int InsertBatch(Poco::Data::Session& session, std::string batchRef, std::string sku , int qty)
    {
        session << R"(INSERT INTO batches (reference, sku, _purchased_quantity, eta) VALUES (?, ?, ?, NULL))",
            Poco::Data::Keywords::use(batchRef),
            Poco::Data::Keywords::use(sku),
            Poco::Data::Keywords::use(qty),
            Poco::Data::Keywords::now;

        int id = 0;
        session << R"(SELECT id FROM batches WHERE reference = ? AND sku = ?)",
            Poco::Data::Keywords::into(id),
            Poco::Data::Keywords::use(batchRef),
            Poco::Data::Keywords::use(sku),
            Poco::Data::Keywords::now;

        return id;
    }

    void InsertAllocation(Poco::Data::Session& session, int orderlineId, int batchId)
    {
        session << R"(INSERT INTO allocations (orderline_id, batch_id) VALUES (?, ?))",
            Poco::Data::Keywords::use(orderlineId),
            Poco::Data::Keywords::use(batchId),
            Poco::Data::Keywords::now;
    }

    std::vector<std::string> GetAllocations(Poco::Data::Session& session, std::string batchRef)
    {
        std::vector<std::string> results;

        Poco::Data::Statement select(session);
        select << R"(
            SELECT order_lines.orderid 
            FROM allocations
            JOIN order_lines ON allocations.orderline_id = order_lines.id
            JOIN batches ON allocations.batch_id = batches.id
            WHERE batches.reference = ?
        )",
            Poco::Data::Keywords::use(batchRef),
            Poco::Data::Keywords::into(results),
            Poco::Data::Keywords::now;

        return results;
    }

    std::string RandomSuffix()
    {
        return Poco::UUIDGenerator::defaultGenerator().createRandom().toString().substr(0, 6);
    }

    std::string RandomSku(const std::string& name)
    {
        return "sku-" + name + "-" + RandomSuffix();
    }

    std::string RandomBatchRef(const std::string& name)
    {
        return "batch-" + name + "-" + RandomSuffix();
    }

    std::string RandomOrderId(const std::string& name)
    {
        return "order-" + name + "-" + RandomSuffix();
    }

}