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

#include "updateModule.hh" 
#include "packet.hh"


CLICK_DECLS 
UpdateModule::UpdateModule() : _timerUpdate(this){
  this->updateSequence = 10;
  this->_myAddr = 0;
  this->_delay = 0;
  this->_period = 5;
  this->_timeout = 1;
}

UpdateModule::~UpdateModule(){}

int 
UpdateModule::initialize(){
    return 0;
}


void
UpdateModule::run_timer(Timer* timer){
  if(timer == &_timerUpdate){
    this->sendUpdate();
  }
  _timerUpdate.reschedule_after_sec(this->_period);
}


int
UpdateModule::configure(Vector<String>&conf, ErrorHandler* errh){
 if (cp_va_kparse(conf, this, errh,
              "MY_ADDRESS", cpkP+cpkM, cpUnsigned, &_myAddr,
              "DELAY", cpkP, cpUnsigned, &_delay,
              "PERIOD", cpkP, cpUnsigned, &_period,
              "TIME_OUT", cpkP, cpUnsigned, &_timeout,
              "ACK_TABLE", cpkP+cpkM, cpElement, &ackModule,
              "ROUTING_TABLE", cpkP+cpkM, cpElement, &routingTable,
              cpEnd) < 0) {
  return -1;
  }
  return 0;
}

/* void push()
 * after receiveing update packet, events invoked:
 * -> update routing table
 * -> update forwarding table
 * -> send ack
 */
void 
UpdateModule::push(int port, Packet *packet) {
  assert(packet);
	/* it is a update packet with additional input port info*/
	uint8_t* portNum = (uint8_t*)packet->data();
	struct UpdatePacket* updatePacket = (struct UpdatePacket*)(portNum+1);

 	click_chatter("[MulticastRouter] Receiving Update Packet from Source %d with sequence %d from port %d", updatePacket->sourceAddr, updatePacket->sequenceNumber, *portNum);

  /* update routing table && forwarding table */
  uint16_t routingTableRowCount = updatePacket->length;

  uint16_t* castSrcAddr = new uint16_t;
  uint32_t* castCost = new uint32_t;
  uint16_t* castHopCount = new uint16_t;
  uint16_t* castNextHop = new uint16_t;

  castSrcAddr = (uint16_t*)(updatePacket+1);
  for(uint16_t i = 0; i < routingTableRowCount; i++){
    castCost = (uint32_t*)(castSrcAddr+1);
    castHopCount = (uint16_t*)(castCost+1);
    castNextHop = (uint16_t*)(castHopCount+1);
    
    for(uint16_t i = 0; i< *castHopCount; i++){
        this->routingTable->computeRoutingTable( *castSrcAddr, (*castCost) + 1, *castNextHop); /* castCost + 1 -> 1 hop to its neighbor */
        this->routingTable->computeForwardingTable( *castSrcAddr, (*castCost) + 1, *portNum);
        castNextHop++;
    }

    castSrcAddr = castNextHop;

  }

  delete castSrcAddr;
  delete castCost;
  delete castHopCount;
  delete castNextHop;

  /* send back ack */
  this->sendAck(*portNum, updatePacket->sequenceNumber, updatePacket->sourceAddr);
}

/* sendUpdate()
 * send self routing Table to neighbors
 */
void
UpdateModule::sendUpdate(){


  /* 
   * calculate size of routing table 
   *-------------------------------------------------------
   * HashTable<uint16_t, routingTableParam>routingTable;
   * struct routingTableParam:
   *     uint32_t cost,
   *     uint16_t hopCount; 
   *     List<uint16_t> nextHop; 
   *--------------------------------------------------------
 */
  if(ackModule->ackTable.get(this->updateSequence) == true){
    this->updateSequence++;
  }
  int routingTableSize = 0;
  int routingTableRowCount = 0;
  for(HashTable<uint16_t,struct RoutingTable::routingTableParam*>::iterator it = this->routingTable->routingTable.begin(); it; ++it){
      routingTableSize += ( sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint16_t) * it.value()->hopCount );
      routingTableRowCount++;
  }

  WritablePacket *updatePacket = Packet::make(0,0, sizeof(struct UpdatePacket)+ routingTableSize, 0);
  memset(updatePacket->data(), 0, updatePacket->length());
  struct UpdatePacket *format = (struct UpdatePacket*) updatePacket->data();
  format->type = UPDATE;
  format->sourceAddr = this->_myAddr;
  format->sequenceNumber = this->updateSequence;
  format->length = routingTableRowCount; /* length of payload */

  /* write payload as routing table info, with looping struct pointer to write payload*/

  uint16_t* castSrcAddr = new uint16_t;
  uint32_t* castCost = new uint32_t;
  uint16_t* castHopCount = new uint16_t;
  uint16_t* castNextHop = new uint16_t;

  castSrcAddr = (uint16_t*)(format+1);

  for(HashTable<uint16_t,struct RoutingTable::routingTableParam*>::iterator it = this->routingTable->routingTable.begin(); it; ++it){

    *castSrcAddr = it.key();

    castCost = (uint32_t*)(castSrcAddr+1);
    *castCost = it.value()->cost;

    castHopCount = (uint16_t*)(castCost+1);
    *castHopCount = it.value()->hopCount;

    castNextHop = castHopCount + 1;
    
    for(Vector<uint16_t>::iterator list = it.value()->nextHop.begin(); list; ++list){
       *castNextHop = *list;
       castNextHop++;
    }

    castSrcAddr = castNextHop;
  }

  delete castSrcAddr;
  delete castCost;
  delete castHopCount;
  delete castNextHop;

  output(0).push(updatePacket);

  click_chatter("[MulticastRouter] Sending Update Message with sequence %d", this->updateSequence);
}


void
UpdateModule::sendAck(const uint8_t portNum, const uint8_t sequenceNumber, const uint16_t sourceAddr){

    WritablePacket *ackPacket = Packet::make(0,0, sizeof(AckPacket), 0);
    memset(ackPacket->data(), 0, ackPacket->length());
    struct AckPacket* format = (struct AckPacket*) ackPacket->data();
    format->type = ACK;
    format->sourceAddr = this->_myAddr;
    format->sequenceNumber = sequenceNumber;
    format->destinationAddr = sourceAddr;

    WritablePacket *q = ackPacket->push(sizeof(uint8_t));
    uint8_t *port = (uint8_t*) q->data();
    *port = *((uint8_t*)(&portNum));

    output(1).push(q);
}

CLICK_ENDDECLS 
EXPORT_ELEMENT(UpdateModule)

