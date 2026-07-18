#include "web_server.h"

#include <WebServer.h>

static WebServer server(80);

//------------------------------------------------------------

bool webServerInit()
{
    server.on("/", []()
    {
        server.send(
            200,
            "text/html",
            "<html><body><h2>World Clock</h2><p>Web server is running.</p></body></html>");
    });

    server.begin();

    return true;
}

//------------------------------------------------------------

void webServerLoop()
{
    server.handleClient();
}