#pragma once

#include "Precompile.hpp"

#include "AbstractUnitOfWork.hpp"
#include "Domain/Ports/IRepository.hpp"


namespace Allocation::Services::UoW
{
    /// @brief SQL реализация единицы работы (Unit of Work).
    class SqlUnitOfWork final : public AbstractUnitOfWork
    {
        struct Impl;

    public:
        /// @brief Конструктор.
        SqlUnitOfWork();

        /// @brief Деструктор.
        ~SqlUnitOfWork();

        /// @brief Получение сессии базы данных.
        /// @return Сессию базы данных.
        [[nodiscard]] std::weak_ptr<Poco::Data::Session> GetSession() noexcept override;

        /// @brief Подтверждение изменений.
        void Commit() override;

        /// @brief Откат изменений.
        void RollBack() override;

        /// @brief Получение репозитория продуктов.
        /// @return Ссылка на репозиторий продуктов.
        [[nodiscard]] Domain::IRepository& GetProductRepository() override;

        /// @brief Получение новых сообщений.
        /// @return Вектор новых сообщений.
        [[nodiscard]] std::vector<Domain::IMessagePtr> GetNewMessages() noexcept override;

    private:
        std::unique_ptr<Impl> _impl;
        std::unordered_set<Domain::IMessagePtr> _newMessages;
    };
}