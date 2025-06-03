#include "Server.h"

#include "HandlerFactory.h"
#include "Adapters/Database/Session/SessionPool.h"
#include "Adapters/Database/DbTables.h"


namespace Allocation::Infrastructure::Server
{
    void ServerApp::initialize(Application& self)
    {
        loadConfiguration();
        initDatabase();
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

        auto& logger = Poco::Util::Application::instance().logger();
        logger.information("The server is running");
        server.start();
        waitForTerminationRequest();
        server.stop();
        logger.information("The server is stopped");
    }

    void ServerApp::initDatabase()
    {
        Poco::Data::SQLite::Connector::registerConnector();
        Adapters::Database::SessionPool::Instance().Configure("SQLite", "TestBd");
        auto session = Adapters::Database::SessionPool::Instance().GetSession();
        Adapters::Database::InitDatabase(session);
    }

}