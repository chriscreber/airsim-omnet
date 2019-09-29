#include <omnetpp.h>
#include "NetPkt_m.h"

using namespace omnetpp;

/**
 * Represents the network "cloud" between clients and the server;
 * see NED file for more info.
 */
class Cloud : public cSimpleModule
{
  private:
    simtime_t propDelay;
    int numPorts;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Cloud);

void Cloud::initialize()
{
    propDelay = (double)par("propDelay");
    numPorts = (int)par("numGates");
}

void Cloud::handleMessage(cMessage *msg)
{
    // determine destination address
    NetPkt *pkt = check_and_cast<NetPkt *>(msg);
    int src = pkt->getSrcAddress();
    EV << "Relaying packet to everyone, but addr=" << src << endl;

    // send msg to destination after the delay
    for(int i = 0; i < numPorts; i++) {
        if(i != src) {
            cMessage *copy = pkt->dup();
            sendDelayed(copy, propDelay, "g$o", i);
        }
    }
}
