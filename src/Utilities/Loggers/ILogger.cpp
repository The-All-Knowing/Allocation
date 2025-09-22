#include "ILogger.hpp"


namespace Allocation::Loggers
{
    namespace Internal
    {
        /// @brief Хранит экземпляр логгера.
        class LoggerHolder
        {
        public:
            /// @brief Проверяет инициализирован ли логгер.
            /// @return true - инициализирован, иначе false.
            static bool IsInitialize()
            {
                std::lock_guard<std::mutex> lock(mutex_);
                if (instance_)
                    return true;
                return false;
            }

            /// @brief Устанавливает экземпляр логгера.
            /// @param logger Инициализирующий логгер.
            static void SetLogger(ILoggerPtr logger)
            {
                std::lock_guard<std::mutex> lock(mutex_);
                instance_ = logger;
            }

            /// @brief Получает экземпляр логгера.
            /// @return Логгер.
            /// @throw std::runtime_error Если логгер не инициализирован.
            static ILoggerPtr GetLogger()
            {
                std::lock_guard<std::mutex> lock(mutex_);
                if (!instance_)
                    throw std::runtime_error("Logger not initialized");
                return instance_;
            }

        private:
            static ILoggerPtr instance_;
            static std::mutex mutex_;
        };

        ILoggerPtr LoggerHolder::instance_;
        std::mutex LoggerHolder::mutex_;
    }

    ILoggerPtr GetLogger() { return Internal::LoggerHolder::GetLogger(); }

    void InitializeLogger(ILoggerPtr logger) { Internal::LoggerHolder::SetLogger(logger); }

    bool IsInitialize() { return Internal::LoggerHolder::IsInitialize(); }
}