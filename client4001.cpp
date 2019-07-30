#include <iostream>          
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>


using namespace std;

#define RCVBUFSIZE 200    // Size of receive buffer
#define PORT 4001
#define HOST "127.0.0.1"

int count = 0;

int sock = 0, valread;
struct sockaddr_in serv_addr;
char basePacket[] = "This is packet number ";
char buffer[200] = {0};

void sigHandler(int signum) {
  valread = read(sock, buffer, RCVBUFSIZE);
  std::cout << buffer << endl;
  char packet[RCVBUFSIZE];
  sprintf(packet, "%s%d\r", basePacket, count);
  send(sock, packet, strlen(packet), 0);
  std::cout << "Sending message " << count << endl;
  count++;

}

int main(int argc, char *argv[]) {

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
      printf("\n Socket creation error \n");
      return -1;
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);

  // Convert IPv4 and IPv6 addresses from text to binary form
  if(inet_pton(AF_INET, HOST, &serv_addr.sin_addr)<=0)
  {
      printf("\nInvalid address/ Address not supported \n");
      return -1;
  }

  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
  {
      printf("\nConnection Failed \n");
      return -1;
  }

  signal(SIGIO, sigHandler);
  fcntl(sock, F_SETOWN, getpid());
  int flag;
  flag = fcntl(sock, F_GETFL, 0);
  fcntl(sock, F_SETFL, flag | O_ASYNC);
  int on = 1;
  ioctl(sock, FIOASYNC, &on);

  char packet[RCVBUFSIZE];
  sprintf(packet, "%s%d\r", basePacket, count);
  send(sock, packet, strlen(packet), 0);
  std::cout << "Sending message " << count << endl;
  count++;

  for (;;) {
  }
  return 0;

}
