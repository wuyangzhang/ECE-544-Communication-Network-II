/***********************************************************************
* PacketClient.cc
*   
* stop and wait for ack
*  
 
*      Author: Wuyang Zhang
************************************************************************/
#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/timer.hh>
#include <click/packet.hh>
#include "packet.hh"
#include "packetClient.hh"
CLICK_DECLS 

PacketClient::PacketClient() : _timer(this) {
	sequenceNumber = 1;
}

PacketClient::~PacketClient(){
	
}
int PacketClient::configure(Vector<String>&conf, ErrorHandler *errh){
    if(cp_va_kparse(conf, this, errh, 
      "SrcAddr", cpkM, cpString, &this->SrcAddr, 
      "DestAddr", cpkM, cpString, &this->DestAddr,
      cpEnd) < 0 )return -1;
}

int PacketClient::initialize(ErrorHandler *errh){
    if(_timer.initialize(this) == false){
        return errh->error("timer initiate failure!");
    }
    _timer.schedule_now();
    return 0;
}

void PacketClient::run_timer(Timer *timer) {

    assert(timer == &_timer);
    this->sendRequest();
    this->helloMessage();
    /* waiting for 2s, periodically send hello message.*/  
    _timer.reschedule_after_msec(2000);
}

void PacketClient::sendRequest(){
    /*if get the ack, send the next sequence, otherwise resend the current one*/  
    if(ackSequenceTable.get(sequenceNumber) == true){
        this->sequenceNumber++;
    }

    WritablePacket *packet = Packet::make(0,0,sizeof(struct PacketHeader)+5, 0);
    memset(packet->data(),0,packet->length());
    struct PacketHeader *format = (struct PacketHeader*) packet->data();
    format->type = 0;
    format->sequenceNumber = this->sequenceNumber;
    char *data = (char*)(packet->data()+sizeof(struct PacketHeader));
    memcpy(data, "zzzzz", 5);
    output(0).push(packet);
    click_chatter("Sending request message sequence %d", this->sequenceNumber);

}

void PacketClient::push(int, Packet *p){
    struct PacketHeader *header = (struct PacketHeader*) p->data();
    uint8_t type = header->type;
    uint32_t seq = header->sequenceNumber;
    click_chatter("Receiving an ack message sequence %d", seq);
    /*if type = 1, receive an ack message*/
    if (type == 1)
        if(seq == sequenceNumber)
                ackSequenceTable.set(seq,true);
} 

void PacketClient::helloMessage(){
    WritablePacket *helloPacket = Packet::make(0,0,sizeof(struct PacketHeader)+5, 0);
    memset(helloPacket->data(), 0, helloPacket->length());
    struct PacketHeader *format = (struct PacketHeader*) helloPacket->data();
    format->type = 3;
    output(0).push(helloPacket);

    click_chatter("Sending out Hello Message");
}
CLICK_ENDDECLS 
EXPORT_ELEMENT(PacketClient)