#include "Views.hpp"


namespace Allocation::Services::Views
{
    std::vector<std::pair<std::string, std::string>> Allocations(
        std::string orderid, Domain::IUnitOfWork& uow)
    {
        std::vector<std::pair<std::string, std::string>> results;
        auto session = uow.GetSession().lock();
        if (!session)
            throw results;

        Poco::Data::Statement select(*session);
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