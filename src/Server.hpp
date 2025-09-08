#pragma once

#include "Precompile.hpp"

#include "Entrypoints/Redis/RedisListener.hpp"
#include "Adapters/Database/DatabaseConfig.hpp"
#include "Adapters/Redis/RedisConfig.hpp"


namespace Allocation
{
    /// @brief Серверное приложение, использующее Poco::Util::ServerApplication.
    class ServerApp : public Poco::Util::ServerApplication
    {
    protected:
        /// @brief Инициализирует приложения.
        /// @param self Ссылка на текущее приложение.
        void initialize(Application& self) override;

        /// @brief Определяет параметры командной строки.
        /// @param options Набор параметров.
        void defineOptions(Poco::Util::OptionSet& options) override;

        /// @brief Главная функция приложения.
        /// @param args Аргументы командной строки.
        /// @return Код завершения приложения.
        int main(const std::vector<std::string>&) override;

    private:
        /// @brief Обрабатывает команду "Help".
        /// @param name Название команды.
        /// @param value Значение.
        void HandleHelp(const std::string& name, const std::string& value);

        /// @brief Обрабатывает команду "Config".
        /// @param name Название команды.
        /// @param value Значение.
        void HandlePathToConfig(const std::string& name, const std::string& value);
        
        /// @brief Запускает сервер.
        void StartServer();

        /// @brief Инициализирует параметры сервера.
        void InitServer();

        /// @brief Инициализирует шину сообщений.
        void InitMessageBus();

        /// @brief Инициализирует базу данных.
        void InitDatabase();

        /// @brief Инициализирует клиент Redis.
        void InitRedis();

        /// @brief Загружает параметры БД из файла конфига.
        /// @return Конфигурация подключения к СУБД.
        Adapters::Database::DatabaseConfig LoadDatabaseConfigFromFile();

        /// @brief Загружает параметры Redis из файла конфига.
        /// @return Конфигурация подключения к Redis.
        Adapters::Redis::RedisConfig LoadRedisConfigFromFile();

        /// @brief Загружает конфигурацию сервера из файла.
        /// @return Параметры сервера, порт.
        std::pair<Poco::Net::HTTPServerParams*, Poco::UInt16> LoadServerConfigFromFile();

        bool _helpRequested{false};
        bool _isConfigFileLoaded{false};
        Poco::Net::HTTPServerParams* _serverParameters;
        Poco::UInt16 _port;
        Entrypoints::Redis::RedisListenerPtr _redisListener;
    };
}
