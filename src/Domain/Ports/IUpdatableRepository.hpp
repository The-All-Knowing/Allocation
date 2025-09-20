#pragma once

#include "IRepository.hpp"


namespace Allocation::Domain
{
    /// @brief Расширенный интерфейс репозитория для обновления агрегатов.
    class IUpdatableRepository : public IRepository
    {
    public:
        /// @brief Деструктор.
        virtual ~IUpdatableRepository() = default;

        /// @brief Обновляет агрегат-продукт в репозитории.
        /// @param product Агрегат-продукт для обновления.
        /// @param oldVersion Изначальная версия агрегата, загруженная из репозитория.
        virtual void Update(ProductPtr product, int oldVersion) = 0;

    protected:
        /// @brief Конструктор.
        IUpdatableRepository() = default;
    };
}