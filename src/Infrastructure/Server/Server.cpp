#include "Server.h"

#include "HandlerFactory.h"
#include "Adapters/Database/Session/SessionPool.h"
#include "Adapters/Database/DbTables.h"
#include "Infrastructure/Redis/RedisListenerModule.h"
#include "Services/Loggers/PocoLogger.h"


namespace Allocation::Infrastructure::Server
{
    void ServerApp::initialize(Application& self)
    {
        Services::Loggers::InitializeLogger(std::make_shared<Services::Loggers::PocoLogger>());
        loadConfiguration();
        initDatabase();
        addSubsystem(new Redis::RedisListenerModule());
        ServerApplication::initialize(self);
    }

    void ServerApp::defineOptions(Poco::Util::OptionSet& options)
    {
        ServerApplication::defineOptions(options);

        options.addOption(
            Poco::Util::Option("help", "h", "Show help")
                .required(false)
                .repeatable(false)
                .callback(Poco::Util::OptionCallback<ServerApp>(this, &ServerApp::handleHelp)));
    } 

    int ServerApp::main(const std::vector<std::string>&)
    {
        if (_helpRequested)
            return Application::EXIT_OK;

        setupAndRunServer();
        return Application::EXIT_OK;
    };

    void ServerApp::handleHelp(const std::string& name, const std::string& value)
    {
        Poco::Util::HelpFormatter helpFormatter(options());
        helpFormatter.setCommand(commandName());
        helpFormatter.setUsage("OPTIONS");
        helpFormatter.setHeader("Order Management Service.");
        helpFormatter.format(std::cout);
        stopOptionsProcessing();
        _helpRequested = true;
    }

    void ServerApp::setupAndRunServer()
    {
        const auto& cfg = config();

        Poco::UInt16 port = cfg.getInt("server.port", 9980);
        Poco::Net::HTTPServerParams* pParams = new Poco::Net::HTTPServerParams;
        pParams->setMaxQueued(100);
        pParams->setMaxThreads(16);
        pParams->setMaxKeepAliveRequests(cfg.getInt("server.max_connections", 100));

        Poco::Net::ServerSocket serverSocket(port);
        Poco::Net::HTTPServer server(new HandlerFactory, serverSocket, pParams);

        Services::Loggers::GetLogger()->Information("The server is running");
        server.start();
        waitForTerminationRequest();
        server.stop();
        Services::Loggers::GetLogger()->Information("The server is stopped");
    }

    void ServerApp::initDatabase()
    {
        const auto& cfg = config();
        
        Adapters::Database::ConnectionConfig config;
        std::string dbHost = cfg.getString("database.host", "localhost");
        int dbPort = cfg.getInt("database.port", 5432);
        std::string dbname = cfg.getString("database.name", "Allocation");
        std::string user = cfg.getString("database.username", "postgres");
        std::string password = cfg.getString("database.password", "1");

        std::ostringstream oss;
        oss << "host=" << dbHost
            << " port=" << dbPort
            << " dbname=" << dbname
            << " user=" << user
            << " password=" << password;

        config.connTimeout = cfg.getInt("database.connection_timeout", 60);
        config.connector = Poco::Data::PostgreSQL::Connector::KEY;
        config.connectionString = oss.str();

        Adapters::Database::SessionPool::Instance().Configure(config);
        Poco::Data::PostgreSQL::Connector::registerConnector();

        auto session = Adapters::Database::SessionPool::Instance().GetSession();
        Adapters::Database::InitDatabase(session);
    }
}