#include "includes/src/Core/Client.h"
#include "includes/src/Core/Server.h"
#include "includes/src/Core/Proxy.h"

int main() {
     // Create Proxy Socket
     ProxyServer Proxy(8000);
     // Create Servers


     // Ready to receive connections
     Proxy.Run();
}
