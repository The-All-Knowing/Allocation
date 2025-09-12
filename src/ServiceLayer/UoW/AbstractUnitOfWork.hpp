#pragma once

#include "Adapters/Repository/TrackingRepository.hpp"
#include "Domain/Ports/IUnitOfWork.hpp"

namespace Allocation::ServiceLayer::UoW
{
    /// @brief Абстрактный базовый класс для реализации паттерна "Единица работы" (Unit of Work).
    /// Отвечает за контроль транзакций и отслеживание изменений продуктов через TrackingRepository.
    class AbstractUnitOfWork : public Domain::IUnitOfWork
    {
    public:
        /// @brief Конструктор.
        /// @param repo Репозиторий продуктов, который будет обернут TrackingRepository
        /// для отслеживания изменений и версионности.
        explicit AbstractUnitOfWork(Domain::IUpdatableRepository& repo);

        /// @brief Подтверждает все изменения.
        void Commit() override;

        /// @brief Откатывает все изменения.
        void RollBack() override;

        /// @brief Проверяет, были ли изменения подтверждены.
        /// @return true, если изменения подтверждены через Commit(), иначе false.
        bool IsCommited() const noexcept override;

        /// @brief Возвращает репозиторий продуктов.
        [[nodiscard]] Domain::IRepository& GetProductRepository() override;

        /// @brief Возвращает все новые сообщения, сгенерированные продуктами
        /// в рамках текущей единицы работы.
        /// @return Сгенерированные сообщения.
        [[nodiscard]] std::vector<Domain::IMessagePtr> GetNewMessages() noexcept override;

    private:
        Adapters::Repository::TrackingRepository _tracking;
        bool _isCommited{false};
    };
}
