#include "Views.hpp"


namespace Allocation::ServiceLayer::Views
{
    std::vector<std::pair<std::string, std::string>> Allocations(
        std::string orderid, Domain::IUnitOfWork& uow)
    {
        std::vector<std::pair<std::string, std::string>> results;
        auto session = uow.GetSession();
        Poco::Data::Statement select(session);
        select << "SELECT sku, batchref FROM allocation.allocations_view WHERE orderid = $1",
            Poco::Data::Keywords::use(orderid), Poco::Data::Keywords::now;

        Poco::Data::RecordSet rs(select);
        for (bool more = rs.moveFirst(); more; more = rs.moveNext())
            results.emplace_back(rs["sku"].toString(), rs["batchref"].toString());
        return results;
    }
}