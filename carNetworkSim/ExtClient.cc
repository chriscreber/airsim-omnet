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

class ExtClient : public cSimpleModule
{
  private:
    int srcAddr1, srcAddr2;
    int destAddr1, destAddr2;

    cMessage *rtEvent;
    cMessage *rtEvent2;
    cSocketScheduler *rtScheduler = nullptr;

    // There is quite literally no other reason to include "refresher" except for the fact that nothing would work without it.
    // What I mean is that the system will not move along without some consistent
    // "refresher" event to essentially nudge it and tell it to keep moving.
    // I don't know why
    // I would like to remove it
    // But I cannot
    cMessage *refresher;

    char recvBuffer[BUFFERSIZE];
    char *recvBufferPtr;
    int numRecvBytes;

    char recvBuffer2[BUFFERSIZE];
    char *recvBufferPtr2;
    int numRecvBytes2;

  public:
    ExtClient();
    virtual ~ExtClient();

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    void sendHTTPRequest();
    void handleSocketEvent();
    void handleSocketEvent2();
    void handleReply(HTTPMsg *httpReply);
    void handleReply2(HTTPMsg *httpReply);

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
    // The following print statement is changed in between runs to make sure Omnet actually accepts my code when it compiles
    // I don't know why, but sometime it just runs the code from the previous build so it's just my way to double check
    std::cout << "working" << endl;
    if(rtScheduler == nullptr) {
        refresher = new cMessage("refresher");
        rtScheduler = check_and_cast<cSocketScheduler*>(getSimulation()->getScheduler());
    }
    if (strcmp("extClient1", getName()) == 0) {
        rtEvent = new cMessage("rtEvent");
        rtScheduler->setInterfaceModule(this, rtEvent, recvBuffer, BUFFERSIZE, &numRecvBytes);
        recvBufferPtr = recvBuffer;
        srcAddr1 = par("srcAddr");
        destAddr1 = par("destAddr");
    } else if (strcmp("extClient2", getName()) == 0) {
        rtEvent2 = new cMessage("rtEvent2");
        rtScheduler->setInterfaceModule2(this, rtEvent2, recvBuffer2, BUFFERSIZE, &numRecvBytes2);
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
        scheduleAt(simTime()+2.5, refresher);
    } else if (strcmp("extClient1", getName()) == 0) {
        handleReply(check_and_cast<HTTPMsg *>(msg));
    } else if (strcmp("extClient2", getName()) == 0) {
        handleReply2(check_and_cast<HTTPMsg *>(msg));
    }
}

void ExtClient::handleSocketEvent()
{
    // try to find a \r -- I just chose that ya know... because.
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

    // assemble and send HTTP request
    HTTPMsg *httpMsg = new HTTPMsg();
    httpMsg->setPayload(header.c_str());
    httpMsg->setDestAddress(destAddr1);
    httpMsg->setSrcAddress(srcAddr1);

    recvBufferPtr += SLOTSIZE;
    if (SLOTSIZE + recvBufferPtr - recvBuffer >= BUFFERSIZE) {
        recvBufferPtr = recvBuffer;
    }

    send(httpMsg, "g$o");
}

void ExtClient::handleSocketEvent2()
{
    // try to find a \r -- I just chose that ya know... because.
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

    // assemble and send HTTP request
    HTTPMsg *httpMsg = new HTTPMsg();
    httpMsg->setPayload(header.c_str());
    httpMsg->setDestAddress(destAddr2);
    httpMsg->setSrcAddress(srcAddr2);

    recvBufferPtr2 += SLOTSIZE;
    if (SLOTSIZE + recvBufferPtr2 - recvBuffer2 >= BUFFERSIZE) {
        recvBufferPtr2 = recvBuffer2;
    }

    send(httpMsg, "g$o");
}

void ExtClient::handleReply(HTTPMsg *httpReply)
{
    const char *reply = httpReply->getPayload();
    rtScheduler->sendBytes(reply, strlen(reply));
    delete httpReply;
}

void ExtClient::handleReply2(HTTPMsg *httpReply)
{
    const char *reply = httpReply->getPayload();
    rtScheduler->sendBytes2(reply, strlen(reply));
    delete httpReply;
}

