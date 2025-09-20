#pragma once

#include "IMessage.hpp"
#include "IRepository.hpp"


namespace Allocation::Domain
{
    /// @brief Интерфейс единицы работы.
    class IUnitOfWork
    {
    public:
        /// @brief Деструктор.
        virtual ~IUnitOfWork() = default;

        /// @brief Подтверждает изменения.
        virtual void Commit() = 0;

        /// @brief Откатывает изменения.
        virtual void RollBack() = 0;

        /// @brief Проверяет, были ли изменения зафиксированы.
        [[nodiscard]] virtual bool IsCommited() const noexcept = 0;

        /// @brief Возвращает сессию подключения к базе данных.
        /// @return Сессию подключения к базе данных.
        [[nodiscard]] virtual Poco::Data::Session GetSession() noexcept = 0;

        /// @brief Возвращает репозиторий хранилища продуктов.
        /// @return Репозиторий продуктов.
        [[nodiscard]] virtual IRepository& GetProductRepository() = 0;

        /// @brief Возвращает новые сообщения из обработанных агрегатов.
        /// @return Новые сообщения порождённые в агрегатах.
        [[nodiscard]] virtual std::vector<IMessagePtr> GetNewMessages() noexcept = 0;
    };
}