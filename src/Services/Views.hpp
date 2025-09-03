#pragma once

#include "Precompile.hpp"

#include "Domain/Ports/IUnitOfWork.hpp"


namespace Allocation::Services::Views
{
    /// @brief Получает список аллокаций по идентификатору заказа.
    /// @param orderid Идентификатор заказа.
    /// @param uow Единица работы.
    /// @return Список пар (артикул, ссылка на партию) для аллокаций.
    std::vector<std::pair<std::string, std::string>> Allocations(
        std::string orderid, Domain::IUnitOfWork& uow);
}