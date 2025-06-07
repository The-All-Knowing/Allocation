#pragma once

#include "Precompile.h"


namespace Allocation::Infrastructure::Server
{
    
    class ServerApp : public Poco::Util::ServerApplication
    {
    protected:
        void initialize(Application& self) override;
        void defineOptions(Poco::Util::OptionSet& options) override;
        int main(const std::vector<std::string>&) override;

    private:
        void handleHelp(const std::string& name, const std::string& value);
        void setupAndRunServer();
        void initDatabase();

        bool _helpRequested {false};
    };
}
