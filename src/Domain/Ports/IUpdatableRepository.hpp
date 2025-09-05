#pragma once

#include "IRepository.hpp"


namespace Allocation::Domain
{
    /// @brief Интерфейс репозитория с поддержкой обновления и контроля версий.
    /// 
    /// Используется в тех случаях, когда необходимо работать с изменяющимися объектами
    /// и отслеживать их версии (например, через TrackingRepository или UnitOfWork).
    class IUpdatableRepository : public IRepository
    {
    public:
        /// @brief Деструктор.
        virtual ~IUpdatableRepository() = default;

        /// @brief Обновляет продукт в репозитории с проверкой версии (optimistic locking).
        /// @param product Продукт для обновления.
        /// @param oldVersion Версия продукта, которая должна совпасть с текущей в репозитории.
        ///                   Если не указана, будет использоваться версия из самого продукта.
        /// @throw std::invalid_argument Если product == nullptr.
        /// @throw std::runtime_error Если произошёл конфликт версий (версия в БД не совпала).
        virtual void Update(ProductPtr product, std::optional<int> oldVersion = std::nullopt) = 0;

    protected:
        /// @brief Конструктор.
        IUpdatableRepository() = default;
    };
}