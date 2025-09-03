#include "Server.hpp"


int main(int argc, char** argv)
{
    setlocale(LC_ALL, "RUS");

    Allocation::ServerApp app;
    return app.run(argc, argv);
}