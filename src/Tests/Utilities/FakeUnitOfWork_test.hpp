#pragma once

#include "FakeRepository_test.hpp"
#include "ServiceLayer/UoW/AbstractUnitOfWork.hpp"


namespace Allocation::Tests
{
    /// @brief FakeStatementImpl — тестовый фейк, не выполняющий реальных SQL-запросов
    class FakeStatementImpl : public Poco::Data::StatementImpl
    {
    public:
        explicit FakeStatementImpl(Poco::Data::SessionImpl& rSession) : StatementImpl(rSession) {}
        ~FakeStatementImpl() override = default;

    protected:
        std::size_t columnsReturned() const override { return 0; }
        int affectedRowCount() const override { return 0; }
        const Poco::Data::MetaColumn& metaColumn(std::size_t /*pos*/) const override
        {
            throw Poco::NotImplementedException("FakeStatementImpl::metaColumn");
        }
        bool hasNext() override { return false; }
        std::size_t next() override { return 0; }
        bool canBind() const override { return false; }
        bool canCompile() const override { return false; }
        void compileImpl() override {}
        void bindImpl() override {}
        void execDirectImpl(const std::string&) override {}
        Poco::Data::AbstractExtraction::ExtractorPtr extractor() override
        {
            return Poco::Data::AbstractExtraction::ExtractorPtr();
        }
        Poco::Data::AbstractBinding::BinderPtr binder() override
        {
            return Poco::Data::AbstractBinding::BinderPtr();
        }
        bool isStoredProcedure() const override { return false; }
    };

    /// @brief Фейковая реализация SessionImpl для тестирования.
    class FakeSessionImpl : public Poco::Data::SessionImpl
    {
    public:
        explicit FakeSessionImpl(const std::string& connectionString)
            : Poco::Data::SessionImpl(connectionString)
        {
            setDBMSName("FakeDB");
        }

        ~FakeSessionImpl() override = default;

        void open(const std::string& = "") override {}
        void close() override {}
        bool isConnected() const override { return true; }
        void setConnectionTimeout(std::size_t) override {}
        std::size_t getConnectionTimeout() const override { return 0; }
        Poco::SharedPtr<Poco::Data::StatementImpl> createStatementImpl() override
        {
            return new FakeStatementImpl(*this);
        }
        void reset() override {}
        void begin() override {}
        void commit() override {}
        void rollback() override {}
        bool canTransact() const override { return false; }
        bool isTransaction() const override { return false; }
        void setTransactionIsolation(Poco::UInt32) override {}
        Poco::UInt32 getTransactionIsolation() const override { return 0; }
        bool hasTransactionIsolation(Poco::UInt32) const override { return false; }
        bool isTransactionIsolation(Poco::UInt32) const override { return false; }
        const std::string& connectorName() const override
        {
            static std::string name = "FakeConnector";
            return name;
        }
        bool hasFeature(const std::string&) const override { return false; }
        void setFeature(const std::string&, bool) override {}
        bool getFeature(const std::string&) const override { return false; }
        bool hasProperty(const std::string&) const override { return false; }
        void setProperty(const std::string&, const Poco::Any&) override {}
        Poco::Any getProperty(const std::string&) const override { return Poco::Any{}; }
    };

    /// @brief Фейковая реализация единицы работы для тестирования.
    class FakeUnitOfWork final : public ServiceLayer::UoW::AbstractUnitOfWork
    {
    public:
        /// @brief Конструктор.
        FakeUnitOfWork() : AbstractUnitOfWork(_repo) {}

        /// @brief Возвращает сессию подключения к базе данных.
        /// @return Фейковая сессия подключения к базе данных.
        [[nodiscard]] Poco::Data::Session GetSession() noexcept
        {
            return Poco::Data::Session(new FakeSessionImpl("connection_string"));
        }

    private:
        FakeRepository _repo;
    };
}
