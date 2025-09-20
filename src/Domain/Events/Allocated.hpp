#pragma once

#include "AbstractEvent.hpp"


namespace Allocation::Domain::Events
{
    /// @brief Событие "Распределена позиция заказа".
    struct Allocated final : public AbstractEvent
    {
        /// @brief Конструктор.
        /// @param orderid Идентификатор заказа клиента.
        /// @param sku Артикул товара товара заказа.
        /// @param batchref Ссылка на партию в которой находится заказ.
        /// @param qty Количество распределённого товара.
        Allocated(std::string orderid, std::string sku, size_t qty, std::string batchref)
            : orderid(std::move(orderid)),
              sku(std::move(sku)),
              qty(qty),
              batchref(std::move(batchref)){};

        /// @brief Возвращает имя события.
        /// @return Имя события.
        [[nodiscard]] std::string Name() const override { return "Allocated"; };

        std::string orderid;
        std::string sku;
        size_t qty;
        std::string batchref;
    };

    /// @brief Читает аттрибуты события.
    /// @param event Событие.
    /// @return Атрибуты события: название - значение.
    template <>
    inline std::vector<EventAttribute> GetAttributes<Allocated>(EventPtr event)
    {
        auto p = std::static_pointer_cast<Allocated>(event);
        return {{"orderid", p->orderid}, {"sku", p->sku}, {"qty", std::to_string(p->qty)},
            {"batchref", p->batchref}};
    };
}