//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <omnetpp/platdep/sockets.h>
#include <omnetpp.h>
#include "HttpMsg_m.h"
#include "SocketScheduler.h"
#include <typeinfo>

using namespace omnetpp;
using namespace std;

#define SLOTSIZE 200

class ExtClient : public cSimpleModule
{
  private:
    int srcAddr1, srcAddr2;
    int destAddr1, destAddr2;

    cMessage *rtEvent;
    cMessage *rtEvent2;
    cMessage *refresher;
    cSocketScheduler *rtScheduler = nullptr;

    char recvBuffer[4000];
    char *recvBufferPtr;
    int numRecvBytes;

    char recvBuffer2[4000];
    char *recvBufferPtr2;
    int numRecvBytes2;

  public:
    ExtClient();
    virtual ~ExtClient();

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    void sendHTTPRequest();
    void processHTTPReply(HTTPMsg *httpMsg);
    void handleSocketEvent();
    void handleSocketEvent2();
    void handleReply(HTTPMsg *httpReply);

};

Define_Module(ExtClient);

ExtClient::ExtClient()
{
    rtEvent = nullptr;
}

ExtClient::~ExtClient()
{
    cancelAndDelete(rtEvent);
}

void ExtClient::initialize()
{
    std::cout << "work" << endl;
    if(rtScheduler == nullptr) {
        refresher = new cMessage("refresher");
        rtScheduler = check_and_cast<cSocketScheduler*>(getSimulation()->getScheduler());
    }
    if (strcmp("extClient1", getName()) == 0) {
        rtEvent = new cMessage("rtEvent");
        rtScheduler->setInterfaceModule(this, rtEvent, recvBuffer, 4000, &numRecvBytes);
        recvBufferPtr = recvBuffer;
        srcAddr1 = par("srcAddr");
        destAddr1 = par("destAddr");
    } else if (strcmp("extClient2", getName()) == 0) {
        rtEvent2 = new cMessage("rtEvent2");
        rtScheduler->setInterfaceModule2(this, rtEvent2, recvBuffer2, 4000, &numRecvBytes2);
        recvBufferPtr2 = recvBuffer2;
        srcAddr2 = par("srcAddr");
        destAddr2 = par("destAddr");
    }
    scheduleAt(simTime(), refresher);
}

void ExtClient::handleMessage(cMessage *msg)
{
    if (msg == rtEvent) {
        handleSocketEvent();
    } else if (msg == rtEvent2) {
        handleSocketEvent2();
    } else if (msg == refresher) {
        scheduleAt(simTime()+5.0, refresher);
    } else if (strcmp("extClient1", getName()) == 0) {
        handleReply(check_and_cast<HTTPMsg *>(msg));
    } else if (strcmp("extClient2", getName()) == 0) {
        processHTTPReply(check_and_cast<HTTPMsg *>(msg));
    }
}

//void ExtClient::handleSocketEvent()
//{
//    // try to find a double line feed in the input -- that's the end of the HTTP header.
//    //char *endHeader = nullptr;
//    //EV << "Dis is duh recvBuffer: " << recvBuffer << " :Dis is duh end of recvBuffer" << endl;
//    EV << "Dest addr: " << destAddr1 << endl;
//    EV << "scr addr: " << srcAddr1 << endl;
//    // assemble and send HTTP request
//    HTTPMsg *httpMsg = new HTTPMsg();
//    httpMsg->setPayload(recvBuffer);
//    httpMsg->setDestAddress(destAddr1);
//    httpMsg->setSrcAddress(srcAddr1);
//
//    send(httpMsg, "g$o");
//}

void ExtClient::handleSocketEvent()
{
    // try to find a double line feed in the input -- that's the end of the HTTP header.
    char *endHeader = nullptr;
    for (char *s = recvBufferPtr; s <= recvBufferPtr+numRecvBytes-1; s++)
        if (*s == '\r') {
            endHeader = s+1;
            break;
        }

    // we don't have a complete header yet -- keep on waiting
    if (!endHeader)
        return;
    std::string header = std::string(recvBufferPtr, endHeader-recvBufferPtr);
    // EV << header;

    // remove HTTP header from buffer
//    if (endHeader == recvBuffer+numRecvBytes) {
//        EV << "bad place" << endl;
//        numRecvBytes = 0;
//    }
//    else {
//        int bytesLeft = recvBuffer+numRecvBytes-endHeader;
//        memmove(endHeader, recvBuffer, bytesLeft);
//        numRecvBytes = bytesLeft;
//    }

    std::cout << "Dest addr: " << destAddr1 << endl;
    std::cout << "scr addr: " << srcAddr1 << endl;

    // assemble and send HTTP request
    HTTPMsg *httpMsg = new HTTPMsg();
    httpMsg->setPayload(header.c_str());
//    httpMsg->setPayload(recvBuffer);
    httpMsg->setDestAddress(destAddr1);
    httpMsg->setSrcAddress(srcAddr1);

    recvBufferPtr += SLOTSIZE;

    send(httpMsg, "g$o");
}

void ExtClient::handleSocketEvent2()
{
    // try to find a double line feed in the input -- that's the end of the HTTP header.
    char *endHeader = nullptr;
    for (char *s = recvBufferPtr2; s <= recvBufferPtr2+numRecvBytes2-1; s++)
        if (*s == '\r') {
            endHeader = s+1;
            break;
        }

    // we don't have a complete header yet -- keep on waiting
    if (!endHeader)
        return;
    std::string header = std::string(recvBufferPtr2, endHeader-recvBufferPtr2);
    // EV << header;

    // remove HTTP header from buffer
//    if (endHeader == recvBuffer2+numRecvBytes2) {
//        EV << "bad place" << endl;
//        numRecvBytes2 = 0;
//    }
//    else {
//        int bytesLeft = recvBuffer2+numRecvBytes2-endHeader;
//        memmove(endHeader, recvBuffer2, bytesLeft);
//        numRecvBytes2 = bytesLeft;
//    }

    std::cout << "Dest addr2: " << destAddr2 << endl;
    std::cout << "scr addr2: " << srcAddr2 << endl;

    // assemble and send HTTP request
    HTTPMsg *httpMsg = new HTTPMsg();
    httpMsg->setPayload(header.c_str());
//    httpMsg->setPayload(recvBuffer2);
    httpMsg->setDestAddress(destAddr2);
    httpMsg->setSrcAddress(srcAddr2);

    recvBufferPtr2 += SLOTSIZE;

    send(httpMsg, "g$o");
}

void ExtClient::sendHTTPRequest()
{
    const char *header = "GET / HTTP/1.0\r\n\r\n";
    // assemble and send HTTP request
    HTTPMsg *httpMsg = new HTTPMsg();
    httpMsg->setPayload(header);
    httpMsg->setDestAddress(destAddr2);
    httpMsg->setSrcAddress(srcAddr2);

    send(httpMsg, "g$o");
}

//void ExtClient::handleReply(HTTPMsg *httpReply)
//{
//    const char *reply = "World";
//    rtScheduler->sendBytes(reply, strlen(reply));
//    delete httpReply;
//}

void ExtClient::handleReply(HTTPMsg *httpReply)
{
    const char *reply = httpReply->getPayload();
    EV << "reply for sendBytes: " << reply << " ---end" << endl;
    rtScheduler->sendBytes(reply, strlen(reply));
    delete httpReply;
}

void ExtClient::processHTTPReply(HTTPMsg *httpReply)
{
    const char *reply = httpReply->getPayload();
//    const char *reply = "World";
    EV << "reply for sendBytes2: " << reply << " ---end" << endl;
    rtScheduler->sendBytes2(reply, strlen(reply));
    delete httpReply;
}

