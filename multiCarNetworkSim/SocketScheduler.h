#ifndef __CSOCKETSCHEDULER_H__
#define __CSOCKETSCHEDULER_H__

#include <omnetpp/platdep/sockets.h>
#include <omnetpp.h>

using namespace omnetpp;

#define BUFFERSIZE 4000
#define BASEPORT 4001
#define NUMPORTS 30
#define SLOTSIZE 200

class cSocketScheduler : public cScheduler
{
  protected:
    // config
    int port[30];
//    int port2;

    cModule *module[30];
    cMessage *notificationMsg[30];
    char *recvBuffer[30];
    char *recvBufferPtr[30];
    int recvBufferSize[30];
    int *numBytesPtr[30];

//    cModule *module2;
//    cMessage *notificationMsg2;
//    char *recvBuffer2;
//    char *recvBufferPtr2;
//    int recvBufferSize2;
//    int *numBytesPtr2;

    // state
    int64_t baseTime; // in microseconds, as returned by opp_get_monotonic_clock_usecs()
    SOCKET listenerSocket[30];
    SOCKET connSocket[30];

//    SOCKET listenerSocket2;
//    SOCKET connSocket2;

    virtual void setupListener();
    virtual void setupListener2();
    virtual bool receiveWithTimeout(long usec);
    virtual int receiveUntil(int64_t targetTime);

  public:
    /**
     * Constructor.
     */
    cSocketScheduler();

    /**
     * Destructor.
     */
    virtual ~cSocketScheduler();

    /**
     * Return a description for the GUI.
     */
    virtual std::string info() const override;

    /**
     * Called at the beginning of a simulation run.
     */
    virtual void startRun() override;

    /**
     * Called at the end of a simulation run.
     */
    virtual void endRun() override;

    /**
     * Recalculates "base time" from current wall clock time.
     */
    virtual void executionResumed() override;

    /**
     * To be called from the module which wishes to receive data from the
     * socket. The method must be called from the module's initialize()
     * function.
     */
    virtual void setInterfaceModule(cModule *module, cMessage *notificationMsg,
            char *recvBuffer, int recvBufferSize, int *numBytesPtr, int i);

    /**
     * To be called from the module which wishes to receive data from the
     * socket. The method must be called from the module's initialize()
     * function.
     */
    virtual void setInterfaceModule2(cModule *module, cMessage *notificationMsg,
            char *recvBuffer, int recvBufferSize, int *numBytesPtr);

    /**
     * Returns the first event in the Future Event Set.
     */
    virtual cEvent *guessNextEvent() override;

    /**
     * Scheduler function -- it comes from the cScheduler interface.
     */
    virtual cEvent *takeNextEvent() override;

    /**
     * Undo takeNextEvent() -- it comes from the cScheduler interface.
     */
    virtual void putBackEvent(cEvent *event) override;

    /**
     * Send on the currently open connection
     */
    virtual void sendBytes(const char *buf, size_t numBytes, int thisSrc);

    /**
     * Send on the currently open connection
     */
    virtual void sendBytes2(const char *buf, size_t numBytes);
};

#endif
