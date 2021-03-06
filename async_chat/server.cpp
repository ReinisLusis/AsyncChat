#include "server/server_app.h"

int main(int argc, char ** argv)
{
    try {
        async_chat::ServerApp app(argc, argv);
        return app.run();
    } catch( std::exception & e)
    {
        std::cout << e.what() << std::endl;
    }
    return 0;
}