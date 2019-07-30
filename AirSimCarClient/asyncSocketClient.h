#ifndef ASYNC_SOCKET_CLIENT_H_
#define ASYNC_SOCKET_CLIENT_H_

#ifdef _cplusplus
extern "C" {
#endif

int setupSocket(int port);
int sendPacket(const char *packet);

#ifdef _cplusplus
}
#endif

#endif
