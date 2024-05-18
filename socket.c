#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <icmpapi.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")

#define PACKET_SIZE 64

unsigned short calculate_checksum(void *b, int len)
{
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2)
    {
        sum += *buf++;
    }
    if (len == 1)
    {
        sum += *(unsigned char *)buf;
    }
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

void create_icmp_echo_request(ICMP_ECHO_REPLY *icmp_hdr)
{
    icmp_hdr->DataSize = PACKET_SIZE;
    icmp_hdr->Data = malloc(PACKET_SIZE);
    memset(icmp_hdr->Data, 0, PACKET_SIZE);
    icmp_hdr->Options.Ttl = 128;
}

void print_icmp_packet(const ICMP_ECHO_REPLY *icmp_hdr)
{
    printf("\nICMP Packet\n");
    printf("\t|-RoundTripTime : %ld ms\n", icmp_hdr->RoundTripTime);
    printf("\t|-DataSize : %d bytes\n", icmp_hdr->DataSize);
    printf("\t|-Options Ttl : %ld\n", icmp_hdr->Options.Ttl);
    printf("\t|---\n");
}

int main()
{
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

    HANDLE hIcmpFile;
    unsigned long ipaddr = inet_addr("8.8.8.8");
    DWORD dwRetVal = 0;
    char SendData[PACKET_SIZE] = "Data Buffer";
    LPVOID ReplyBuffer = NULL;
    DWORD ReplySize = 0;

    hIcmpFile = IcmpCreateFile();
    if (hIcmpFile == INVALID_HANDLE_VALUE)
    {
        printf("\tUnable to open handle.\n");
        printf("IcmpCreatefile returned error: %ld\n", GetLastError());
        WSACleanup();
        return 1;
    }

    ReplySize = sizeof(ICMP_ECHO_REPLY) + PACKET_SIZE;
    ReplyBuffer = (VOID *)malloc(ReplySize);
    if (ReplyBuffer == NULL)
    {
        printf("\tUnable to allocate memory\n");
        WSACleanup();
        return 1;
    }

    dwRetVal = IcmpSendEcho(hIcmpFile, ipaddr, SendData, sizeof(SendData), NULL, ReplyBuffer, ReplySize, 1000);
    if (dwRetVal != 0)
    {
        PICMP_ECHO_REPLY pEchoReply = (PICMP_ECHO_REPLY)ReplyBuffer;
        struct in_addr ReplyAddr;
        ReplyAddr.S_un.S_addr = pEchoReply->Address;
        printf("\tSent ICMP message to %s\n", "8.8.8.8");
        printf("\tReceived %ld ICMP message response\n", dwRetVal);
        printf("\tRoundtrip time: %ld ms\n", pEchoReply->RoundTripTime);
        print_icmp_packet(pEchoReply);
    }
    else
    {
        printf("\tCall to IcmpSendEcho failed.\n");
        printf("IcmpSendEcho returned error: %ld\n", GetLastError());
    }

    free(ReplyBuffer);
    IcmpCloseHandle(hIcmpFile);
    WSACleanup();
    return 0;
}
