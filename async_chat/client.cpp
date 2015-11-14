#include "client/client_app.h"

int main(int argc, char ** argv)
{
    try {
        ClientApp app(argc, argv);
        return app.run();
    } catch( std::exception & e)
    {
        std::cout << e.what() << std::endl;
    }
}