#pragma once

#include "Precompile.h"


namespace Allocation::Adapters::Database
{


    class FakeSessionImpl : public Poco::Data::SessionImpl
    {
    public:
        FakeSessionImpl() : Poco::Data::SessionImpl("fake", 0) {}

        const std::string & connectorName() const override {return "";};

        bool hasFeature(const std::string & name) const override {return false;};
        bool hasProperty(const std::string & name) const override {return false;};

        void begin() override { _inTransaction = true; }
        void commit() override { _inTransaction = false; }
        void rollback() override { _inTransaction = false; }

        void setTransactionIsolation(Poco::UInt32) override {}
        Poco::UInt32 getTransactionIsolation() const override {return 0;}

        void setConnectionTimeout(std::size_t timeout) override {}
        std::size_t getConnectionTimeout() const override {return 1;}

        bool isTransaction() const override { return _inTransaction; }
        bool canTransact() const override {return false;};

        void open(const std::string & connectionString = "") override {};
        void reset() override {};
        void close() override { _isConnected = false; }
        bool isConnected() const override { return _isConnected; }

        void setFeature(const std::string&, bool) override {}
        bool getFeature(const std::string&) const override { return false; }

        void setProperty(const std::string&, const Poco::Any&) override {}
        Poco::Any getProperty(const std::string&) const override { return Poco::Any(); }

        bool hasTransactionIsolation(Poco::UInt32) const override {return false;};
        bool isTransactionIsolation(Poco::UInt32) const override {return false;};

        Poco::Data::StatementImpl::Ptr createStatementImpl() override {
            return nullptr;
        }

    private:
        bool _inTransaction = false;
        bool _isConnected = true;
    };

}