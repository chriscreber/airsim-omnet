#include "SocketScheduler.h"

Register_Class(cSocketScheduler);

inline std::ostream& operator<<(std::ostream& out, const timeval& tv)
{
    return out << (unsigned long)tv.tv_sec << "s" << tv.tv_usec << "us";
}

cSocketScheduler::cSocketScheduler() : cScheduler()
{
    for(int i = 0; i < NUMPORTS; i++) {
        listenerSocket[i] = INVALID_SOCKET;
        connSocket[i] = INVALID_SOCKET;
    }

//    listenerSocket2 = INVALID_SOCKET;
//    connSocket2 = INVALID_SOCKET;
}

cSocketScheduler::~cSocketScheduler()
{
}

std::string cSocketScheduler::info() const
{
    return "socket RT scheduler";
}

void cSocketScheduler::startRun()
{
    if (initsocketlibonce() != 0)
        throw cRuntimeError("cSocketScheduler: Cannot initialize socket library");

    baseTime = opp_get_monotonic_clock_usecs();

    for(int i = 0; i < NUMPORTS; i++) {
        module[i] = nullptr;
        notificationMsg[i] = nullptr;
        recvBuffer[i] = nullptr;
        recvBufferSize[i] = 0;
        numBytesPtr[i] = nullptr;
        port[i] = BASEPORT + i;
    }


    setupListener();

//    baseTime = opp_get_monotonic_clock_usecs();
//
//    module2 = nullptr;
//    notificationMsg2 = nullptr;
//    recvBuffer2 = nullptr;
//    recvBufferSize2 = 0;
//    numBytesPtr2 = nullptr;
//
//    port2 = 4002;
//    setupListener2();
}

void cSocketScheduler::setupListener()
{
    for(int i = 0; i < NUMPORTS; i++) {
        // Creating socket file descriptor
        if ((listenerSocket[i] = socket(AF_INET, SOCK_STREAM, 0)) == 0)
        {
            throw cRuntimeError("cSocketScheduler: cannot create socket");
        }
        int enable = 1;
        if (setsockopt(listenerSocket[i], SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &enable, sizeof(int)))
        {
            throw cRuntimeError("cSocketScheduler: cannot set socket option");
        }
        sockaddr_in sinInterface;
        sinInterface.sin_family = AF_INET;
        sinInterface.sin_addr.s_addr = INADDR_ANY;
        sinInterface.sin_port = htons(port[i]);
        if (bind(listenerSocket[i], (sockaddr *)&sinInterface, sizeof(sockaddr_in)) == SOCKET_ERROR)
            throw cRuntimeError("cSocketScheduler: socket bind() failed");

        listen(listenerSocket[i], SOMAXCONN);
    }

}

void cSocketScheduler::setupListener2()
{
//    // Creating socket file descriptor
//    if ((listenerSocket2 = socket(AF_INET, SOCK_STREAM, 0)) == 0)
//    {
//        throw cRuntimeError("cSocketScheduler: cannot create socket2");
//    }
//    int enable = 1;
//    if (setsockopt(listenerSocket2, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &enable, sizeof(int)))
//    {
//        throw cRuntimeError("cSocketScheduler: cannot set socket option2");
//    }
//    sockaddr_in sinInterface;
//    sinInterface.sin_family = AF_INET;
//    sinInterface.sin_addr.s_addr = INADDR_ANY;
//    sinInterface.sin_port = htons(port2);
//    if (bind(listenerSocket2, (sockaddr *)&sinInterface, sizeof(sockaddr_in)) == SOCKET_ERROR)
//        throw cRuntimeError("cSocketScheduler: socket bind() failed2");
//
//    listen(listenerSocket2, SOMAXCONN);
}

void cSocketScheduler::endRun()
{
}

void cSocketScheduler::executionResumed()
{
    baseTime = opp_get_monotonic_clock_usecs();
    baseTime = baseTime - simTime().inUnit(SIMTIME_US);
}

void cSocketScheduler::setInterfaceModule(cModule *mod, cMessage *notifMsg, char *buf, int bufSize, int *nBytesPtr, int i)
{
    if (module[i])
        throw cRuntimeError("cSocketScheduler: setInterfaceModule() already called");
    if (!mod || !notifMsg || !buf || !bufSize || !nBytesPtr)
        throw cRuntimeError("cSocketScheduler: setInterfaceModule(): arguments must be non-nullptr");

    module[i] = mod;
    notificationMsg[i] = notifMsg;
    recvBuffer[i] = buf;
    recvBufferPtr[i] = buf;
    recvBufferSize[i] = bufSize;
    numBytesPtr[i] = nBytesPtr;
    *numBytesPtr[i] = 0;
}

void cSocketScheduler::setInterfaceModule2(cModule *mod, cMessage *notifMsg, char *buf, int bufSize, int *nBytesPtr)
{
//    if (module2)
//        throw cRuntimeError("cSocketScheduler: setInterfaceModule() already called");
//    if (!mod || !notifMsg || !buf || !bufSize || !nBytesPtr)
//        throw cRuntimeError("cSocketScheduler: setInterfaceModule(): arguments must be non-nullptr");
//
//    module2 = mod;
//    notificationMsg2 = notifMsg;
//    recvBuffer2 = buf;
//    recvBufferPtr2 = buf;
//    recvBufferSize2 = bufSize;
//    numBytesPtr2 = nBytesPtr;
//    *numBytesPtr2 = 0;
}

bool cSocketScheduler::receiveWithTimeout(long usec)
{
    bool retValue = false;
    fd_set readFDs;
    FD_ZERO(&readFDs);

    for(int i = 0; i < NUMPORTS; i++) {
        // if we're connected, watch connSocket, otherwise accept new connections
        if (connSocket[i] != INVALID_SOCKET) {
            FD_SET(connSocket[i], &readFDs);
        } else {
            FD_SET(listenerSocket[i], &readFDs);
        }
    }

//    // if we're connected, watch connSocket, otherwise accept new connections
//    if (connSocket2 != INVALID_SOCKET) {
//        FD_SET(connSocket2, &readFDs);
//    } else {
//        FD_SET(listenerSocket2, &readFDs);
//    }

    timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = usec;

    if (select(FD_SETSIZE, &readFDs, NULL, NULL, &timeout) > 0) {
        // Something happened on one of the sockets -- handle them
        for(int i = 0; i < NUMPORTS; i++) {
            if (connSocket[i] != INVALID_SOCKET && FD_ISSET(connSocket[i], &readFDs)) {
                // receive from connSocket
                if (SLOTSIZE + recvBufferPtr[i] - recvBuffer[i] >= BUFFERSIZE) {
                    recvBufferPtr[i] = recvBuffer[i];
                }
                int nBytes = recv(connSocket[i], recvBufferPtr[i], SLOTSIZE, 0);
                recvBufferPtr[i] += SLOTSIZE;


                if (nBytes == SOCKET_ERROR) {
                    EV << "cSocketScheduler: socket error " << sock_errno() << "\n";
                    closesocket(connSocket[i]);
                    connSocket[i] = INVALID_SOCKET;
                }
                else if (nBytes == 0) {
                    EV << "cSocketScheduler: socket closed by the client\n";
                    if (shutdown(connSocket[i], SHUT_WR) == SOCKET_ERROR)
                        throw cRuntimeError("cSocketScheduler: shutdown() failed");
                    closesocket(connSocket[i]);
                    connSocket[i] = INVALID_SOCKET;
                }
                else {
                    // schedule notificationMsg for the interface module
                    EV << "cSocketScheduler: received " << nBytes << " bytes\n";
                    (*numBytesPtr[i]) += nBytes;

                    int64_t currentTime = opp_get_monotonic_clock_usecs();
                    simtime_t eventTime(currentTime - baseTime, SIMTIME_US);
                    ASSERT(eventTime >= simTime());
                    notificationMsg[i]->setArrival(module[i]->getId(), -1, eventTime);
                    getSimulation()->getFES()->insert(notificationMsg[i]);
                    retValue = true;
                }
            }
            else if (FD_ISSET(listenerSocket[i], &readFDs)) {
                // accept connection, and store FD in connSocket
                sockaddr_in sinRemote;
                int addrSize = sizeof(sinRemote);
                connSocket[i] = accept(listenerSocket[i], (sockaddr *)&sinRemote, (socklen_t *)&addrSize);
                if (connSocket[i] == INVALID_SOCKET)
                    throw cRuntimeError("cSocketScheduler: accept() failed");
                EV << "cSocketScheduler: connected! " << i << "\n";
            }
        }


//        if (connSocket2 != INVALID_SOCKET && FD_ISSET(connSocket2, &readFDs)) {
//            // receive from connSocket
//            if (SLOTSIZE + recvBufferPtr2 - recvBuffer2 >= BUFFERSIZE) {
//                recvBufferPtr2 = recvBuffer2;
//            }
//            int nBytes = recv(connSocket2, recvBufferPtr2, SLOTSIZE, 0);
//            recvBufferPtr2 += SLOTSIZE;
//
//
//            if (nBytes == SOCKET_ERROR) {
//                EV << "cSocketScheduler: socket2 error " << sock_errno() << "\n";
//                closesocket(connSocket2);
//                connSocket2 = INVALID_SOCKET;
//            }
//            else if (nBytes == 0) {
//                EV << "cSocketScheduler: socket2 closed by the client\n";
//                if (shutdown(connSocket2, SHUT_WR) == SOCKET_ERROR)
//                    throw cRuntimeError("cSocketScheduler: shutdown()2 failed");
//                closesocket(connSocket2);
//                connSocket2 = INVALID_SOCKET;
//            }
//            else {
//                // schedule notificationMsg for the interface module
//                EV << "cSocketScheduler: received " << nBytes << " bytes2\n";
//                (*numBytesPtr2) += nBytes;
//
//                int64_t currentTime = opp_get_monotonic_clock_usecs();
//                simtime_t eventTime(currentTime - baseTime, SIMTIME_US);
//                ASSERT(eventTime >= simTime());
//                notificationMsg2->setArrival(module2->getId(), -1, eventTime);
//                getSimulation()->getFES()->insert(notificationMsg2);
//                retValue = true;
//            }
//        }
//        else if (FD_ISSET(listenerSocket2, &readFDs)) {
//            // accept connection, and store FD in connSocket
//            sockaddr_in sinRemote;
//            int addrSize = sizeof(sinRemote);
//            connSocket2 = accept(listenerSocket2, (sockaddr *)&sinRemote, (socklen_t *)&addrSize);
//            if (connSocket2 == INVALID_SOCKET)
//                throw cRuntimeError("cSocketScheduler: accept()2 failed");
//            EV << "cSocketScheduler: connected2!\n";
//        }
    }
    return retValue;
}

int cSocketScheduler::receiveUntil(int64_t targetTime)
{
    // if there's more than 200ms to wait, wait in 100ms chunks
    // in order to keep UI responsiveness by invoking getEnvir()->idle()
    int64_t currentTime = opp_get_monotonic_clock_usecs();
    while (targetTime - currentTime >= 200000)
    {
        if (receiveWithTimeout(100000))  // 100ms
            return 1;

        // update simtime before calling envir's idle()
        currentTime = opp_get_monotonic_clock_usecs();
        simtime_t eventTime(currentTime - baseTime, SIMTIME_US);
        ASSERT(eventTime >= simTime());
        sim->setSimTime(eventTime);
        if (getEnvir()->idle())
            return -1;
        currentTime = opp_get_monotonic_clock_usecs();
    }

    // difference is now at most 100ms, do it at once
    long remaining = targetTime - currentTime;
    if (remaining > 0)
        if (receiveWithTimeout(remaining))
            return 1;

    return 0;
}

cEvent *cSocketScheduler::takeNextEvent()
{
    // assert that we've been configured
    if (!module)
        throw cRuntimeError("cSocketScheduler: setInterfaceModule() not called: it must be called from a module's initialize() function");
    // calculate target time
    int64_t targetTime;
    cEvent *event = sim->getFES()->peekFirst();

    if (!event) {
        // if there are no events, wait until something comes from outside
        // TBD: obey simtimelimit, cpu-time-limit
        // This way targetTime will always be "as far in the future as possible", considering
        // how integer overflows work in conjunction with comparisons in C++ (in practice...)
        targetTime = opp_get_monotonic_clock_usecs() + INT64_MAX;
    }
    else {
        // use time of next event
        simtime_t eventSimtime = event->getArrivalTime();
        targetTime = baseTime + eventSimtime.inUnit(SIMTIME_US);
    }

    // if needed, wait until that time arrives
    int64_t currentTime = opp_get_monotonic_clock_usecs();
    if (targetTime > currentTime) {
        int status = receiveUntil(targetTime);
        if (status == -1)
            return nullptr;  // interrupted by user
        if (status == 1)
            event = sim->getFES()->peekFirst();  // received something
    }
    else {
        // we're behind -- customized versions of this class may
        // alert if we're too much behind, whatever that means
    }

    // remove event from FES and return it
    cEvent *tmp = sim->getFES()->removeFirst();
    ASSERT(tmp == event);
    return event;
}

void cSocketScheduler::sendBytes(const char *buf, size_t numBytes, int thisSrc)
{
    if (connSocket[thisSrc] == INVALID_SOCKET) {
        char myerror[100] = {0};
        sprintf(myerror, "cSocketScheduler: sendBytes(): no connection %d", thisSrc);
        throw cRuntimeError(myerror);
    }

    int retval = send(connSocket[thisSrc], buf, numBytes, 0);
    EV << "retval: " << retval << endl;
    // TBD check for errors
}

void cSocketScheduler::sendBytes2(const char *buf, size_t numBytes)
{
//    if (connSocket2 == INVALID_SOCKET)
//        throw cRuntimeError("cSocketScheduler: sendBytes(): no connection");
//
//    int retval = send(connSocket2, buf, numBytes, 0);
//    EV << "retval2: " << retval << endl;
    // TBD check for errors
}

//seems unnecessary

void cSocketScheduler::putBackEvent(cEvent *event)
{
    sim->getFES()->putBackFirst(event);
}

cEvent *cSocketScheduler::guessNextEvent()
{
    return sim->getFES()->peekFirst();
}





