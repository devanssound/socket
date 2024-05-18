#include <af_irda.h>
#include <in6addr.h>
#include <mstcpip.h>
#include <mswsock.h>
#include <nsemail.h>
#include <nspapi.h>
#include <sporder.h>
#include <stdio.h>
#include <winsock.h>
#include <winsock2.h>
#include <ws2atm.h>
#include <ws2spi.h>
#include <ws2tcpip.h>
#include <wsipv6ok.h>
#include <wsnwlink.h>
#include <wsrm.h>

int main() {

    WSADATA winSocketData;
    int iCheck = WSAStartup(MAKEWORD(2, 2), &winSocketData);

    if (iCheck != 0) {
        printf("WSA Startup failed: %d\n", iCheck);
        return 1;
    }

    WSACleanup();

    return 0;
}