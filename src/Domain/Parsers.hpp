#pragma once

#include "Precompile.hpp"

#include "Commands/Allocate.hpp"
#include "Commands/ChangeBatchQuantity.hpp"
#include "Commands/CreateBatch.hpp"


namespace Allocation::Domain
{
    /// @brief Универсальный шаблон преобразования JSON в доменный объект.
    ///
    /// Базовый шаблон является заглушкой и должен быть специализирован для
    /// конкретных доменных типов.
    /// @endcode
    template <typename T>
    IMessagePtr FromJson(Poco::JSON::Object::Ptr)
    {
        static_assert(sizeof(T) == 0, "FromJson<T> must be specialized for the given type T");
        return {};
    }

    /// @brief Преобразует JSON-объект в команду CreateBatch.
    /// @param json JSON-объект с полями ref, sku, qty и опционально eta.
    /// @throw std::invalid_argument если отсутствуют обязательные поля или они некорректны.
    /// @return Умный указатель на команду CreateBatch.
    template <>
    IMessagePtr FromJson<Commands::CreateBatch>(Poco::JSON::Object::Ptr json);

    /// @brief Преобразует JSON-объект в команду Allocate.
    /// @param json JSON-объект с полями orderid, sku и qty.
    /// @throw std::invalid_argument если отсутствуют обязательные поля или они некорректны.
    /// @return Умный указатель на команду Allocate.
    template <>
    IMessagePtr FromJson<Commands::Allocate>(Poco::JSON::Object::Ptr json);

    /// @brief Преобразует JSON-объект в команду ChangeBatchQuantity.
    /// @param json JSON-объект с полями batchref и qty.
    /// @throw std::invalid_argument если отсутствуют обязательные поля или они некорректны.
    /// @return Умный указатель на команду ChangeBatchQuantity.
    template <>
    IMessagePtr FromJson<Commands::ChangeBatchQuantity>(Poco::JSON::Object::Ptr json);
}