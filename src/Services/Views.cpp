#include "Views.hpp"


namespace Allocation::Services::Views
{
    std::vector<std::pair<std::string, std::string>> Allocations(
        std::string orderid, std::shared_ptr<Services::UoW::SqlUnitOfWork> uow)
    {
        std::vector<std::pair<std::string, std::string>> results;
        Poco::Data::Statement select(uow->GetSession());

        select << "SELECT sku, batchref FROM public.allocations_view WHERE orderid = $1",
            Poco::Data::Keywords::use(orderid), Poco::Data::Keywords::now;

        Poco::Data::RecordSet rs(select);
        bool more = rs.moveFirst();
        while (more)
        {
            results.emplace_back(rs["sku"].toString(), rs["batchref"].toString());
            more = rs.moveNext();
        }

        return results;
    }
}