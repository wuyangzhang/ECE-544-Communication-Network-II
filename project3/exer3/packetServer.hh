#ifndef CLICK_PACKETSERVER_HH 
#define CLICK_PACKETSERVER_HH 
#include <click/element.hh>
#include <click/hashtable.hh>
#include <click/timer.hh>
#include <click/string.hh>
CLICK_DECLS
class PacketServer : public Element {
    public:
        PacketServer();
       ~PacketServer(); 
        const char *class_name() const { return "PacketServer";}
        const char *port_count() const { return "1/1";}
        const char *processing() const { return PUSH; }

        void run_timer(Timer*);
        int initialize(ErrorHandler*);
        int configure(Vector<String>&, ErrorHandler*);
        void push(int ,Packet*);
	    void helloMessage();
	private:
        Timer _timer;
        uint32_t srcAddr;


};

CLICK_ENDDECLS
#endif
