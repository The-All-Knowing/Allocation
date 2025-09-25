#pragma once

#include "Commands/Allocate.hpp"
#include "Commands/ChangeBatchQuantity.hpp"
#include "Commands/CreateBatch.hpp"


namespace Allocation::Domain
{
    /// @brief Шаблонная функция для парсинга JSON в доменный объект.
    template <typename T>
    IMessagePtr FromJson(Poco::JSON::Object::Ptr)
    {
        static_assert(sizeof(T) == 0, "FromJson<T> must be specialized for the given type T");
        return {};
    }

    /// @brief Парсит JSON-объект в команду "Создать партию поставки".
    /// @param json JSON-объект с полями ref, sku, qty и опционально eta.
    /// @throw std::invalid_argument если отсутствуют обязательные поля или они некорректны.
    /// @return Команда "Создать партию поставки".
    template <>
    IMessagePtr FromJson<Commands::CreateBatch>(Poco::JSON::Object::Ptr json);

    /// @brief Парсит JSON-объект в команду "Распределить позицию заказа".
    /// @param json JSON-объект с полями orderid, sku и qty.
    /// @throw std::invalid_argument если отсутствуют обязательные поля или они некорректны.
    /// @return Команда "Распределить позицию заказа".
    template <>
    IMessagePtr FromJson<Commands::Allocate>(Poco::JSON::Object::Ptr json);

    /// @brief Парсит JSON-объект в команду "Изменить количество товара в партии заказа".
    /// @param json JSON-объект с полями batchref и qty.
    /// @throw std::invalid_argument если отсутствуют обязательные поля или они некорректны.
    /// @return Команда "Изменить количество товара в партии заказа".
    template <>
    IMessagePtr FromJson<Commands::ChangeBatchQuantity>(Poco::JSON::Object::Ptr json);
}