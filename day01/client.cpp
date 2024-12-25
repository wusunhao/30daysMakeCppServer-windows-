#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <winsock2.h>
#include <cstring>
#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA data;
    if (WSAStartup(MAKEWORD(2, 2), &data) != 0) {
        printf("WSAStartup失败\n");
        return 1;
    }

    SOCKET socketword = socket(AF_INET, SOCK_STREAM, 0);
    if (socketword == INVALID_SOCKET) {
        printf("套接字创建失败: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    struct sockaddr_in serv_addr;
    ZeroMemory(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(8888);

    if (connect(socketword, (sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        printf("连接失败: %d\n", WSAGetLastError());
        closesocket(socketword);
        WSACleanup();
        return 1;
    }

    printf("已成功连接到服务器!\n");

    closesocket(socketword);
    WSACleanup();
    return 0;
}