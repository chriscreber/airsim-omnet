//
// Generated file, do not edit! Created by nedtool 5.5 from NetPkt.msg.
//

#ifndef __NETPKT_M_H
#define __NETPKT_M_H

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0505
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



class NetPkt;
/**
 * Class generated from <tt>NetPkt.msg:14</tt> by nedtool.
 * <pre>
 * //
 * // Packet type that can be relayed by a Cloud.
 * //
 * packet NetPkt
 * {
 *     int srcAddress;
 *     int destAddress;
 * }
 * </pre>
 */
class NetPkt : public ::omnetpp::cPacket
{
  protected:
    int srcAddress = 0;
    int destAddress = 0;

  private:
    void copy(const NetPkt& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const NetPkt&);

  public:
    NetPkt(const char *name=nullptr, short kind=0);
    NetPkt(const NetPkt& other);
    virtual ~NetPkt();
    NetPkt& operator=(const NetPkt& other);
    virtual NetPkt *dup() const override {return new NetPkt(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual int getSrcAddress() const;
    virtual void setSrcAddress(int srcAddress);
    virtual int getDestAddress() const;
    virtual void setDestAddress(int destAddress);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const NetPkt& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, NetPkt& obj) {obj.parsimUnpack(b);}

#endif // ifndef __NETPKT_M_H
