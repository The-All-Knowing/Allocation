#pragma once

#include "Domain/Ports/IUnitOfWork.hpp"


namespace Allocation::Services::UoW
{
    /// @brief Абстрактный класс для реализации единицы работы (Unit of Work).
    class AbstractUnitOfWork : public Domain::IUnitOfWork
    {
    public:
        /// @brief Подтверждение изменений.
        void Commit() override;

        /// @brief Откат изменений.
        void RollBack() override;

        /// @brief Проверяет, были ли изменения подтверждены.
        /// @return true, если изменения подтверждены; в противном случае - false.
        bool IsCommited() const noexcept override;

    private:
        bool _isCommited{false};
    };
}