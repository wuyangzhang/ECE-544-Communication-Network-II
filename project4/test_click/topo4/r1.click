require(library /home/comnetsii/elements/routerport.click);

define($dev1 veth1, $addrDev1 1e:9e:64:0a:54:bd)
define($dev2 veth2, $addrDev2 92:11:66:96:0a:4a)
define($dev3 veth3, $addrDev3 3a:7d:75:61:cc:fa)
define($dev4 veth4, $addrDev4 de:65:c9:c7:aa:29)
define($dev5 veth5, $addrDev5 66:17:6c:45:c9:06)
define($dev6 veth6, $addrDev6 0e:13:20:1d:d4:f3)
define($dev7 veth7, $addrDev7 92:21:63:40:56:e7)
define($dev8 veth8, $addrDev8 5e:af:8d:9a:5d:69)
define($dev9 veth9, $addrDev9 ae:41:f5:cc:c9:04)
define($dev10 veth10, $addrDev10 5a:f9:89:60:3b:34)
define($dev11 veth11, $addrDev11 ee:2d:4c:45:40:3a)
define($dev12 veth12, $addrDev12 8e:84:a1:0a:97:0e)
define($dev13 veth13, $addrDev13 f2:32:2a:2d:b7:5f)
define($dev14 veth14, $addrDev14 42:56:e4:5f:01:57)
define($dev15 veth15, $addrDev15 8a:74:3e:3b:89:d3)
define($dev16 veth16, $addrDev16 4e:e2:4f:e7:e0:ab)
define($dev17 veth17, $addrDev17 aa:81:ae:ae:80:12)
define($dev18 veth18, $addrDev18 3a:51:b8:e3:a2:cf)
define($dev19 veth19, $addrDev19 f2:61:c7:e9:37:39)
define($dev20 veth20, $addrDev20 9a:68:0b:e3:50:de)
define($dev21 veth21, $addrDev21 ca:13:f6:c6:21:d8)
define($dev22 veth22, $addrDev22 ae:30:4f:0d:22:a6)
define($dev23 veth23, $addrDev23 3a:cc:21:5b:5f:fe)
define($dev24 veth24, $addrDev24 56:02:ed:8c:9a:9c)



//********************** Test Topology ***********************************
//
//
//                                        ----[veth4]Router2[veth9]--------[veth10]Router5[veth15]---[veth16]Client1 
//                                       /		                                   [veth21]  
//                                      /				 				              |
//                                 [veth3]			                               [veth22]
// Client0[veth1] -- [veth2]Router1[veth5]----[veth6]Router3[veth11]-------[veth12]Router6[veth17]---[veth18]Client2
//                                 [veth7]										   [veth23]					
//									  \				          						  |
//                                     \										   [veth24]
//										------[veth8]Router4[veth13]-------[veth14]Router5[veth19]---[veth20]Client3
//

// ************************* define router output link ! **********************************************************************
rp0::RouterPort(DEV $dev2, IN_MAC $addrDev2, OUT_MAC $addrDev1)
rp1::RouterPort(DEV $dev3, IN_MAC $addrDev3, OUT_MAC $addrDev4)
rp2::RouterPort(DEV $dev5, IN_MAC $addrDev5, OUT_MAC $addrDev6)
rp3::RouterPort(DEV $dev7, IN_MAC $addrDev7, OUT_MAC $addrDev8)


cl::PacketClassifier()
ack::AckModule()
// ************************* @initiate address ! **********************************************************************
rt::RoutingTable(MY_ADDRESS 1)
hello::HelloModule(MY_ADDRESS 1, DELAY 1, PERIOD 5, TIME_OUT 2, ACK_TABLE ack, ROUTING_TABLE rt)
update::UpdateModule(MY_ADDRESS 1,DELAY 5, PERIOD 5, TIME_OUT 2,  ACK_TABLE ack, ROUTING_TABLE rt)
data::DataModule(ROUTING_TABLE rt)
bd::BroadcastModule()
//------------------------------------------------------------------------------------------------------------------------------

// ************************* @all input ports forward packets to Packet Classifier !*************************
rp0->[0]cl
rp1->[1]cl
rp2->[2]cl
rp3->[3]cl

// packet classifier outport 0: hello, outport1: update, outport2: ack, output3: data
// ack inport 0: receive ack packet, update ack table, inport 1: send out ack
cl[0]->hello
cl[1]->update
cl[2]->[0]ack 
cl[3]->data

// hello outport 0: generate hello packet, send to broadcast, outport 1: receive a hello packet, send ack 
hello[0]->bd
hello[1]->[1]ack 

// update outport 0: generate update packet, send to broadcast, outport 1: receive a update packet, send ack 
update[0]->bd
update[1]->[1]ack



// *************************@ack connect to all valid out port !*************************
ack[0]->rp0
ack[1]->rp1
ack[2]->rp2
ack[3]->rp3

// *************************@broadcast packet to all valid out ports, except access router************************
bd[0]->Discard
bd[1]->rp1
bd[2]->rp2
bd[3]->rp3
bd[4]->Discard

// *************************@forward packet to all valid out ports !*************************
data[0]->rp0
data[1]->rp1
data[2]->rp2
data[3]->rp3
