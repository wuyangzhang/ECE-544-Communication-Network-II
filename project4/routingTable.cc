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

CLICK_DECLS 

RoutingTable::RoutingTable(){
     uint32_t srcAddr = 0;
     uint32_t destAddr = 0;
     uint32_t helloSequence = 0;
     uint32_t updateSequence = 0;
     forwardingTable = HashTable<uint16_t, struct forwardingTableParam*>() ;
     routingTable = HashTable<uint16_t, struct routingTableParam*> ();


}

RoutingTable::~RoutingTable(){
    /* delete routing table && forwarding table */
    for(HashTable<uint16_t,struct RoutingTable::routingTableParam*>::iterator it = this->routingTable.begin(); it != this->routingTable.end(); ++it){
        delete it->value();
    }

    for(HashTable<uint16_t,struct RoutingTable::routingTableParam*>::iterator it = this->forwardingTable.begin(); it != this->forwardingTable.end(); ++it){
        delete it->value();
   }

}

int 
RoutingTable::initialize(ErrorHandler* errh){
    return 0;
}

void 
RoutingTable::push(int port, Packet *packet) {
	
}

/*
void 
RoutingTable::updateRoutingTable(const uint16_t sourceAddr, const uint16_t cost, const uint16_t nextHop){
  
}


void 
RoutingTable::updateForwardingTable(const uint16_t sourceAddr, const uint16_t cost, const uint8_t port){

}

void
RoutingTable::updateRoutingTable(const uint16_t sourceAddr, const uint16_t cost, const uint16_t nextHop, const uint8_t sharedPath){
	
}


void
RoutingTable::updateForwardingTable(const uint16_t sourceAddr, const uint16_t cost, const uint8_t port, const uint8_t sharedPath){

}
*/

/* computeRoutingTable()
 * when a new path to destination presents,
 * if destionation is not in the list, add it
 * new cost < current cost, update value
 * new cost == current cost, record new path
 */
void
RoutingTable::computeForwardingTable(const uint16_t sourceAddr, const uint32_t cost, const uint8_t port){

    struct forwardingTableParam* ftp = new struct forwardingTableParam();

    if(this->forwardingTable.get(sourceAddr) == NULL){
        /* add new destination address */
        ftp->port.push_back(port);
        ftp->portCount = 1;
        ftp->cost = cost;
        this->forwardingTable.set(sourceAddr, ftp);
    }else { /* destiantion exists ! */

        if(cost < this->forwardingTable.get(sourceAddr)->cost){
            /* update the cost of destination */
            ftp->port.push_back(port);
            ftp->portCount = 1;
            ftp->cost = cost;
            this->forwardingTable.set(sourceAddr, ftp);
        }

        if(cost == this->forwardingTable.get(sourceAddr)->cost){
            if(port != this->forwardingTable.get(sourceAddr)->hop.front()){
                /* fetch all current next hop followed by a new next hop, build a new next hop structure */
                ftp->port = this->forwardingTable.get(sourceAddr)->port;
                ftp->port.push_back(port);
                ftp->portCount = ftp->port.size();
                ftp->cost = cost;
                this->forwardingTable.set(sourceAddr, ftp);
            }
            
        }
    }

}


void 
RoutingTable::computeRoutingTable(const uint16_t sourceAddr, const uint32_t cost, const uint16_t nextHop){
    
    struct routingTableParam* rtp = new struct routingTableParam(); 

    if(this->routingTable.get(sourceAddr) == NULL){
    	/* add new destination address */
        rtp->hopCount = 1;
        rtp->cost = cost;
        rtp->nextHop.push_back(nextHop);
        this->routingTable.set(sourceAddr, rtp);


    }else {  /* destiantion exists ! */

        if(cost < this->routingTable.get(sourceAddr)->cost){
            /* update the cost of destination */
            rtp->hopCount = 1;
            rtp->cost = cost;
            rtp->nextHop.push_back(nextHop);
            this->routingTable.set(sourceAddr, rtp); /* reset the cost & next hop for this destination */

        }

        if(cost == this->routingTable.get(sourceAddr)->cost){
            /* fetch all current next hop followed by a new next hop, build a new next hop structure */
            if(nextHop != this->routingTable.get(sourceAddr)->nextHop.front()){
                rtp->nextHop = this->routingTable.get(sourceAddr)->nextHop;
                rtp->nextHop.push_back(nextHop);
                rtp->hopCount = rtp->nextHop.size();
                rtp->cost = cost;
                this->routingTable.set(sourceAddr, rtp);
            }
        
        }
    }


}




int 
RoutingTable::lookUpForwardingTable(uint32_t destAddr){

    int matchPort = -1;
    
    return matchPort;
}


void 
RoutingTable::forwardingPacket(Packet *p, int port){
    
}

void 
RoutingTable::printRoutingTable(){

    for(HashTable<uint16_t,struct RoutingTable::routingTableParam*>::iterator it = this->routingTable.begin(); it != this->routingTable.end(); ++it){
        for(Vector<uint16_t>::iterator list = it.value()->nextHop.begin(); list != it.value()->nextHop.end(); ++list){
            click_chatter("[destinaion] %d | [cost] %d | [next hop] %d \n", it.key(), it.value()->cost, *list);
        }
   }
}


void
RoutingTable::printForwardingTable(){
    for(HashTable<uint16_t,struct RoutingTable::forwardingTableParam*>::iterator it = this->forwardingTable.begin(); it != this->forwardingTable.end(); ++it){
            for(Vector<uint16_t>::iterator list = it.value()->port.begin(); list != it.value()->port.end(); ++list){
                click_chatter("[destinaion] %d | [cost] %d | [next hop] %d \n", it.key(), it.value()->cost, *list);
            }
       }
}

CLICK_ENDDECLS 
EXPORT_ELEMENT(RoutingTable)

