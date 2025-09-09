#pragma once

#include "IRepository.hpp"


namespace Allocation::Domain
{
    /// @brief Интерфейс репозитория с поддержкой обновления и контроля версий.
    ///
    /// Предназначен для работы с изменяемыми агрегатами, состояние которых
    /// необходимо синхронизировать с хранилищем. Поддерживает механизм оптимистической блокировки
    /// (optimistic locking), что позволяет обнаруживать и предотвращать конфликты параллельных
    /// изменений. Используется, например, в TrackingRepository или в рамках Unit of Work.
    class IUpdatableRepository : public IRepository
    {
    public:
        /// @brief Виртуальный деструктор.
        virtual ~IUpdatableRepository() = default;

        /// @brief Обновляет продукт в репозитории с проверкой версии (optimistic locking).
        /// @param product Продукт для обновления.
        /// @param oldVersion Версия продукта, которая должна совпасть с текущей версией в хранилище
        ///                   перед применением изменений.
        /// @throw std::invalid_argument Если product == nullptr.
        /// @throw std::runtime_error Если произошёл конфликт версий (версия в БД не совпала).
        virtual void Update(ProductPtr product, int oldVersion) = 0;

    protected:
        /// @brief Защищённый конструктор для предотвращения создания экземпляров напрямую.
        IUpdatableRepository() = default;
    };
}