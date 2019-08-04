#include "asyncSocketClient.h"

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>

#define RCVBUFSIZE 200    // Size of receive buffer
#define HOST "127.0.0.1"


int sock = 0, valread;
struct sockaddr_in serv_addr;

int setupSocket(int port, void (*signal_handler) (int)) {
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, HOST, &serv_addr.sin_addr)<=0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    signal(SIGIO, signal_handler);
    fcntl(sock, F_SETOWN, getpid());
    int flag;
    flag = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flag | O_ASYNC);
    int on = 1;
    ioctl(sock, FIOASYNC, &on);
    return 0;
}

void readPacket(char *s) {
    valread = read(sock, s, RCVBUFSIZE);
}

void sendPacket(char *packet) {
    send(sock, packet, RCVBUFSIZE, 0);
}

// void readPacket(unsigned char *s, int numBytes) {
//     valread = read(sock, s, numBytes);
// }
//
// void sendPacket(const unsigned char *packet, int numBytes) {
//     send(sock, packet, numBytes, 0);
// }
