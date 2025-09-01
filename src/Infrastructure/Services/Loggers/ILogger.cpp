#include "ILogger.hpp"


namespace Allocation::Services::Loggers
{
    namespace Internal
    {
        class LoggerHolder
        {
        public:
            static void SetLogger(ILoggerPtr logger)
            {
                std::lock_guard<std::mutex> lock(mutex_);
                instance_ = logger;
            }

            static ILoggerPtr GetLogger()
            {
                std::lock_guard<std::mutex> lock(mutex_);
                if (!instance_)
                {
                    throw std::runtime_error("Logger not initialized");
                }
                return instance_;
            }

        private:
            static ILoggerPtr instance_;
            static std::mutex mutex_;
        };
    }

    namespace Internal
    {
        ILoggerPtr LoggerHolder::instance_;
        std::mutex LoggerHolder::mutex_;
    }

    ILoggerPtr GetLogger() { return Internal::LoggerHolder::GetLogger(); }

    void InitializeLogger(ILoggerPtr logger) { Internal::LoggerHolder::SetLogger(logger); }
}