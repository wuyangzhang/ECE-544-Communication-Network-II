/**
  * Created by Wuyang on 4/19/16.
  * Copyright © 2016 Wuyang. All rights reserved.
  * Final Project of ECE 544 Communication Network II 2016 Spring
*/
#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/timer.hh>
#include <click/packet.hh>

#include "routingTable.hh" 
#include "packets.hh"

CLICK_DECLS 
RoutingTable::RoutingTable(){}

RoutingTable::~RoutingTable(){}

int 
RoutingTable::initialize(ErrorHandler *errh){
    return 0;
}

void 
RoutingTable::push(int port, Packet *packet) {
	
}

void 
MulticastRouter::updateRoutingTable(const uint16_t sourceAddr, const uint16_t cost, const uint16_t nextHop){
  
}


void 
MulticastRouter::updateForwardingTable(const uint16_t sourceAddr, const uint16_t cost, const uint8_t port){

}

void
MulticastRouter::updateRoutingTable(const uint16_t sourceAddr, const uint16_t cost, const uint16_t nextHop, const uint8_t sharedPath){
	
}


void
MulticastRouter::updateForwardingTable(const uint16_t sourceAddr, const uint16_t cost, const uint8_t port, const uint8_t sharedPath){

}


/* computeRoutingTable()
 * when a new path to destination presents,
 * if destionation is not in the list, add it
 * new cost < current cost, update value
 * new cost == current cost, record new path
 */
void 
MulticastRouter::computeRoutingTable(const uint16_t destAddr, const uint32_t cost, const uint16_t nextHop){

    struct routingTableParam rtp; 
    rtp.nextHop = List<uint16_t>();
    rtp.cost = cost;

    if(!this->routingTable[destAddr]){
    	/* add new destination address */
    	rtp.nextHop.push_back(nextHop);
        this->forwardingTable.set(sourceAddr, rtp);
    }else if(cost < this->routingTable.get(sourceAddr).cost){
    	/* update the cost of destination */
    	rpt.nextHop.push_back(nextHop);
        this->routingTable.set(sourceAddr, rtp);
    }else if(cost == this->routingTable.get(sourceAddr).cost){
    	/* fetch all current next hop followed by a new next hop, build a new next hop structure */
    	for(List<uint16_t>::iterator it = this->routingTable.get(sourceAddr).nextHop.begin(); it != this->routingTable.get(sourceAddr).nextHop.end(); ++it){
    		rpt.nextHop.push_back(it);
    	}

    	rpt.nextHop.push_back(nextHop);
    }
    /* set hop count of this source address */
    this->routingTable.get(sourceAddr).hopCount = this->routingTable.get(sourceAddr).nextHop.size();
}


void
MulticastRouter::computeForwardingTable(const uint16_t sourceAddr, const uint16_t cost, const uint8_t port){
	struct forwardingTableParam ftp; 
    ftp.port = List<uint8_t>();
    ftp.cost = cost;

    if(!this->forwardingTable[destAddr]){
    	/* add new destination address */
    	ftp.port.push_back(port);
        this->forwardingTable.set(sourceAddr, rtp);
    }else if(cost < this->forwardingTable.get(sourceAddr).cost){
    	/* update the cost of destination */
    	fpt.port.push_back(port);
        this->forwardingTable.set(sourceAddr, ftp);
    }else if(cost == this->forwardingTable.get(sourceAddr).cost){
    	/* fetch all current next hop followed by a new next hop, build a new next hop structure */
    	for(List<uint8_t>::iterator it = this->forwardingTable.get(sourceAddr).port.begin(); it != this->forwardingTable.get(sourceAddr).port.end(); ++it){
    		fpt.nextHop.push_back(it);
    	}

    	fpt.nextHop.push_back(port);
    }

     /* set hop count of this source address */
    this->forwardingTable.get(sourceAddr).portCount = this->forwardingTable.get(sourceAddr).port.size();
}


int 
MulticastRouter::lookUpForwardingTable(uint32_t destAddr){

    int matchPort = -1;
    for(HashTable<int, uint32_t>::iterator it = this->forwardingTable.begin(); it; ++it){
      if(it.value() == destAddr)
            matchPort = it.key();
    }
    return matchPort;
}


void 
MulticastRouter::forwardingPacket(Packet *p, int port){
    output(port).push(p);
    click_chatter("[Router] forwarding packet to port %d", port);
}

void 
MulticastRouter::printRoutingTable(){

}


void
MulticastRouter::printForwardingTable(){

}

CLICK_ENDDECLS 
EXPORT_ELEMENT(RoutingTable)
