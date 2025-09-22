#pragma once

#include "Adapters/Repository/TrackingRepository.hpp"
#include "Domain/Ports/IUnitOfWork.hpp"


namespace Allocation::ServiceLayer::UoW
{
    /// @brief Абстрактный базовый класс для реализации паттерна "Единица работы".
    /// @note Отвечает за контроль транзакций и отслеживание изменений в агрегатах через TrackingRepository.
    class AbstractUnitOfWork : public Domain::IUnitOfWork
    {
    public:
        /// @brief Конструктор.
        /// @param repo Репозиторий, который будет отслеживаться в TrackingRepository.
        explicit AbstractUnitOfWork(Domain::IUpdatableRepository& repo);

        /// @brief Подтверждает изменения.
        void Commit() override;

        /// @brief Откатывает изменения.
        void RollBack() override;

        /// @brief Проверяет, были ли изменения зафиксированы.
        /// @return true, если изменения зафиксированы, иначе false.
        bool IsCommitted() const noexcept override;

        /// @brief Возвращает репозиторий для работы с агрегатами-продуктами.
        [[nodiscard]] Domain::IRepository& GetProductRepository() override;

        /// @brief Возвращает новые сообщения, сгенерированные продуктами
        /// в рамках текущей единицы работы.
        /// @return Сообщения сгенерированные в отслеживаемых агрегатах.
        [[nodiscard]] std::vector<Domain::IMessagePtr> GetNewMessages() noexcept override;

    private:
        Adapters::Repository::TrackingRepository _tracking;
        bool _isCommitted{false};
    };
}
