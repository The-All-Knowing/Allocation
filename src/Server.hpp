#pragma once

#include "Adapters/Database/DatabaseConfig.hpp"
#include "Adapters/Redis/RedisConfig.hpp"
#include "Entrypoints/Redis/RedisListener.hpp"


namespace Allocation
{
    /// @brief Главный класс серверного приложения.
    class ServerApp : public Poco::Util::ServerApplication
    {
    protected:
        /// @brief Инициализирует ресурсы приложения.
        /// @param self Ссылка на текущее приложение.
        void initialize(Application& self) override;

        /// @brief Освобождает ресурсы приложения.
        void uninitialize() override;

        /// @brief Определяет параметры командной строки.
        /// @param options Инициализируемые обработчики параметров.
        void defineOptions(Poco::Util::OptionSet& options) override;

        /// @brief Главная функция приложения.
        /// @param args Аргументы командной строки.
        /// @return Код завершения приложения.
        int main(const std::vector<std::string>&) override;

    private:
        /// @brief Обрабатывает команду "Help".
        /// @param name Название команды.
        void HandleHelp(const std::string& name, const std::string&);

        /// @brief Обрабатывает команду "Config".
        /// @param name Название команды.
        /// @param path Путь к файлу конфигурации.
        void HandlePathToConfig(const std::string& name, const std::string& path);

        /// @brief Запускает основной цикл сервера.
        void StartMainCycle();

        /// @brief Инициализирует параметры сервера.
        void InitServer();

        /// @brief Инициализирует шину сообщений.
        void InitMessageBus();

        /// @brief Инициализирует подключение к базе данных.
        void InitDatabase();

        /// @brief Инициализирует подключение к Redis.
        void InitRedis();

        bool _helpRequested{false};
        Poco::Path _configFile;
        Poco::Net::HTTPServerParams* _serverParameters;
        Poco::UInt16 _port;
        std::unique_ptr<Entrypoints::Redis::RedisListener> _redisListener;
    };
}
