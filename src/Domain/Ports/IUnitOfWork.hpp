#pragma once

#include "Precompile.hpp"

#include "IMessage.hpp"
#include "IRepository.hpp"


namespace Allocation::Domain
{
    /// @brief Интерфейс единицы работы (Unit of Work).
    class IUnitOfWork
    {
    public:
        /// @brief Деструктор.
        virtual ~IUnitOfWork() = default;

        /// @brief Подтверждение изменений.
        virtual void Commit() = 0;

        /// @brief Откат изменений.
        virtual void RollBack() = 0;

        /// @brief Проверка, были ли изменения подтверждены.
        [[nodiscard]] virtual bool IsCommited() const noexcept = 0;

        /// @brief Получение сессии базы данных.
        /// @return Сессию базы данных если реализаия поддерживает, иначе std::nullopt.
        [[nodiscard]] virtual std::optional<Poco::Data::Session> GetSession() noexcept = 0;

        /// @brief Получение репозитория продуктов.
        /// @return Репозиторий продуктов.
        [[nodiscard]] virtual IRepository& GetProductRepository() = 0;

        /// @brief Получение новых сообщений из обработанных продуктов.
        /// @return Вектор указателей на новые сообщения.
        [[nodiscard]] virtual std::vector<IMessagePtr> GetNewMessages() noexcept = 0;
    };
}