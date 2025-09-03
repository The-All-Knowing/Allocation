#pragma once

#include "Precompile.hpp"

#include "Entrypoints/Redis/RedisListener.hpp"


namespace Allocation
{
    /// @brief Серверное приложение, использующее Poco::Util::ServerApplication.
    class ServerApp : public Poco::Util::ServerApplication
    {
    protected:
        /// @brief Инициализация приложения.
        /// @param self Ссылка на текущее приложение.
        void initialize(Application& self) override;

        /// @brief Определение параметров командной строки.
        /// @param options Набор параметров.
        void defineOptions(Poco::Util::OptionSet& options) override;

        /// @brief Главная функция приложения.
        /// @param args Аргументы командной строки.
        /// @return Код завершения приложения.
        int main(const std::vector<std::string>&) override;

    private:
        void HandleHelp(const std::string& name, const std::string& value);
        void SetupAndRunServer();
        void InitDatabase();
        void InitRedis();

        bool _helpRequested{false};
        Entrypoints::Redis::RedisListener RedisListener;
    };
}
