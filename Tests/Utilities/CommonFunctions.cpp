#include "CommonFunctions.h"


namespace Allocation::Tests
{
    std::pair<Domain::Batch, Domain::OrderLine> MakeBatchAndLine(
        const std::string& SKU, size_t batchQty, size_t lineQty)
    {
        return {Domain::Batch("batch-001", SKU, batchQty),
                Domain::OrderLine("order-123", SKU, lineQty)};
    }

    int InsertBatch(Poco::Data::Session& session, std::string batchRef, std::string sku , int qty, int version)
    {
        session << "INSERT INTO public.products (sku, version_number) VALUES ($1, $2)",
            Poco::Data::Keywords::use(sku),
            Poco::Data::Keywords::use(version),
            Poco::Data::Keywords::now;

        session << "INSERT INTO public.batches (reference, sku, purchased_quantity, eta) VALUES ($1, $2, $3, NULL)",
            Poco::Data::Keywords::use(batchRef),
            Poco::Data::Keywords::use(sku),
            Poco::Data::Keywords::use(qty),
            Poco::Data::Keywords::now;

        int id = 0;
        session << "SELECT id FROM public.batches WHERE reference = $1 AND sku = $2",
            Poco::Data::Keywords::into(id),
            Poco::Data::Keywords::use(batchRef),
            Poco::Data::Keywords::use(sku),
            Poco::Data::Keywords::now;

        return id;
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

    std::string GetAllocatedBatchRef(Poco::Data::Session& session, std::string orderid, std::string sku)
    {
        int orderlineId = 0;
        std::string batchref;

        Poco::Data::Statement selectOrderLine(session);
        selectOrderLine << "SELECT id FROM public.order_lines WHERE orderid = $1 AND sku = $2",
                        Poco::Data::Keywords::use(orderid),
                        Poco::Data::Keywords::use(sku),
                        Poco::Data::Keywords::into(orderlineId),
                        Poco::Data::Keywords::now;

        if (selectOrderLine.done() && orderlineId == 0)
            throw std::runtime_error("OrderLine not found.");

         Poco::Data::Statement selectBatchRef(session);
        selectBatchRef << R"(
            SELECT b.reference 
            FROM public.allocations 
            JOIN public.batches AS b ON batch_id = b.id 
            WHERE orderline_id = $1
        )",
        Poco::Data::Keywords::use(orderlineId),
        Poco::Data::Keywords::into(batchref),
        Poco::Data::Keywords::now;

        if (selectBatchRef.done() && batchref.empty())
            throw std::runtime_error("Allocation not found.");

        return batchref;
    }    
}