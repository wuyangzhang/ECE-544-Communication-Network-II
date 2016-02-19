// Parameters:
// my_GUID
// topo_file - GUID-based topology file 
// core_dev - device name e.g., 'eth0' or 'wlan0'
// GNRS_server_port - listening port on server, assumes localhost gnrs
// GNRS_listen_ip - IP assoc w/ interface GNRS listens on
// GNRS_listen_port - response listening port for gnrs clients
// src_GUID - value in source GUID field (uinteger)
// dst_GUID - GUID of destination (uinteger)
// chk_size - size of chunk in bytes
// chk_lmt - total number of chunks to transmit (int; -1 is infinite)
// chk_intval - time spacing between chunks in msec
// delay - seconds to wait before starting 
// pkt_size - size of L2 pkts in bytes
// loss_prob - for testing, between 0 and 1
// service_id - defines delivery type: refer to MF SID values
// window_size - size of sending window
// dst_NAs - destination NAs 

// Maintains router-wide resource stats, etc. 
routerstats::MF_RouterStats();

//Control path elements
arp_tbl::MF_ARPTable;
assoc_tbl::MF_AssocTable();
nbr_tbl::MF_NeighborTable();
rtg_tbl::MF_RoutingTable(MY_GUID $my_GUID, NEIGHBOR_TABLE nbr_tbl);
lp_hndlr::MF_LinkProbeHandler(MY_GUID $my_GUID, NEIGHBOR_TABLE nbr_tbl, ROUTING_TABLE rtg_tbl);
lsa_hndlr::MF_LSAHandler(MY_GUID $my_GUID, NEIGHBOR_TABLE nbr_tbl, ROUTING_TABLE rtg_tbl)
assoc_hndlr::MF_AssocHandler(ASSOC_TABLE assoc_tbl);

//Data path elements
chk_mngr::MF_ChunkManager;
resp_cache::GNRS_RespCache; 
agg::MF_Aggregator(MY_GUID $my_GUID, ROUTER_STATS routerstats, CHUNK_MANAGER chk_mngr);
net_binder::MF_NetworkBinder(RESP_CACHE resp_cache, CHUNK_MANAGER chk_mngr);
intra_lkup::MF_IntraLookUp(MY_GUID $my_GUID, FORWARDING_TABLE rtg_tbl, CHUNK_MANAGER chk_mngr, ASSOC_TABLE assoc_tbl);
seg::MF_Segmentor(routerstats, CHUNK_MANAGER chk_mngr);

//enforces stated topology
topo_mngr::MF_TopologyManager(MY_GUID $my_GUID, TOPO_FILE $topo_file, ARP_TABLE arp_tbl);


//Counters and Statistics
//TODO: build a custom chunk counter to get proper data byte count
//incoming L2 pkt and byte count 
inCntr_pkt::Counter()
//incoming L3 chunk and byte count 
inCntr_chunk::Counter()
//outgoing L2 pkt and byte count 
outCntr_pkt::Counter

//Queues
inQ::ThreadSafeQueue(65535); //incoming L2 pkt Q
net_binderQ::ThreadSafeQueue(1000); //chunk Q prior to NA resolution 
segQ::ThreadSafeQueue(1000); //segmentor ready chunk Q
outQ_ctrl::ThreadSafeQueue(1000); //L2 outgoing high priority control pkt queue
outQ_data::Queue(65535); //L2 outgoing lower priority data pkt queue
outQ_sched::PrioSched; //priority sched for L2 pkts
outQ_core::Queue(65535); //L2 outgoing pkt Q for 'core' port

//L2 packet sources
//core port
fd_core::FromDevice($core_dev, PROMISC false, SNIFFER true, METHOD LINUX);

//L3 chunk sources
//chunk generator 
vchk_src::MF_VirtualChunkSource(SRC_GUID $my_GUID, DST_GUID $dst_GUID, VIRTUAL_NETWORK_GUID $virtual_network_GUID, VIRTUAL_SRC_GUID $virtual_src_GUID, VIRTUAL_DST_GUID $virtual_dst_GUID, ROUTER_STATS routerstats, CHUNK_MANAGER chk_mngr); 

//L2 out i/f
td_core::ToDevice($core_dev);

//Core interface - port 0
fd_core 
        -> HostEtherFilter($core_dev, DROP_OWN true)
	//drop anything that isn't of MF eth type
        -> core_cla::Classifier(12/27C0, -)
        -> SetTimestamp 
        -> MF_Learn(IN_PORT 0, ARP_TABLE arp_tbl) // learn src eth, port
	-> Strip(14) // strip eth header; assumes no VLAN tag
        -> inQ;

core_cla[1] -> Discard;

//start incoming pkt processing
inQ -> inUnq::Unqueue
	-> topo_mngr
	-> inCntr_pkt
	-> mf_cla::Classifier(
			00/00000003, // p0 Link probe
			00/00000004, // p1 Link probe response
			00/00000005, // p2 LSA
			00/00000000, // p3 Data
			00/00000001, // p4 CSYN
			00/00000002, // p5 CSYN-ACK
			00/00000006, // p6 Client association
			00/00000007, // p7 Client dis-association 
			-);          // p8 Unhandled type, discard

mf_cla[7] -> Discard; //No client access in this config
mf_cla[8] -> Discard;

// routing control pkts

mf_cla[0] -> [0]lp_hndlr; // link probe
mf_cla[1] -> [1]lp_hndlr; // link probe ack
mf_cla[2] -> [0]lsa_hndlr; // lsa

// data and Hop signalling pkts

mf_cla[3] -> [0]agg; // data 
mf_cla[4] -> [1]agg; // csyn 
mf_cla[5] -> [1]seg; // csyn-ack
mf_cla[6] -> assoc_hndlr; // host association request

// Net-level processing for aggregated data blocks (chunks)

//chunk generator with fake data
//TODO chunk count here to be accounted to 'host queued'
vchk_src
	-> inCntr_chunk 
	-> net_binderQ;

//chunks assembled post Hop transfer from upstream node
agg[0] 
	-> inCntr_chunk 
	//chunk queue prior to NA resolution
	-> net_binderQ;

net_binderQ 
	-> nbUnq::Unqueue 
	-> [0]net_binder;

net_binder[0] 
	-> svc_cla::Classifier(
			00/00000000, // p0 Default rtg, unicast
            00/00000004, // p1 
	    00/00000800, // p2 virtual
			-)           // p4 Unhandled type, discard 
        //-> intra_lkup; // default rtg
svc_cla[0] -> [0]intra_lkup; 
svc_cla[1] -> [0]intra_lkup;
svc_cla[2] -> [0]intra_lkup;
svc_cla[3] -> Print(WARN_UNHANDLED_SVC_TYPE) -> MF_ChunkSink(CHUNK_MANAGER chk_mngr);

//Forwarding decisions: default routing

intra_lkup[0] -> segQ; //send chunk to next hop

chk_mngr->net_binderQ; 
 
segQ -> segUnq::Unqueue
	-> [0]seg;

//Outgoing csyn/csyn-ack pkts - place in high priority queue 
agg[1] -> outQ_ctrl;

//Outgoing data frame
seg[0] -> outQ_data;

//Rebind chunks that failed transfer to specified downstream node
seg[1] -> net_binderQ;

//Outgoing control pkts
lp_hndlr[0] //outgoing link probe
	-> outQ_ctrl;

lp_hndlr[1] //outgoing link probe ack
	-> outQ_ctrl;

lsa_hndlr[0] //outgoing lsa
	-> outQ_ctrl;

//priority schedule data and control pkts
outQ_ctrl -> [0]outQ_sched; 
outQ_data -> [1]outQ_sched; 

//to switch outgoing L2 pkts to respective learnt ports
out_switch::PaintSwitch(ANNO 41);
outQ_sched 
	-> outCntr_pkt 
	-> swUnq::Unqueue 
	-> paint::MF_Paint(arp_tbl) 
	-> out_switch;

//Send pkts switch to corresponding to-devices
//port 0 is core bound

//core pkts
out_switch[0] 
	-> MF_EtherEncap(0x27c0, $core_dev, ARP_TABLE arp_tbl) 
        -> outQ_core
        -> td_core;

//GNRS insert/update/query handling
//requestor --> request Q --> gnrs client --> gnrs svc
//gnrs svc --> response Q --> gnrs client --> requestor
//Component definitions
// gnrs request queue - multiple requestors
gnrs_reqQ::ThreadSafeQueue(100);
//gnrs client to interact with service
gnrs_rrh::GNRS_ReqRespHandler(MY_GUID $my_GUID, NET_ID "NA", RESP_LISTEN_IP $GNRS_listen_ip, RESP_LISTEN_PORT $GNRS_listen_port, RESP_CACHE resp_cache);
//UDP request sender 
gnrs_svc_sndr::Socket(UDP, $GNRS_server_ip, $GNRS_server_port);
//UDP response listener 
gnrs_svc_lstnr::Socket(UDP, $GNRS_listen_ip, $GNRS_listen_port) 
//gnrs_svc_lstnr::Socket(UDP, 0.0.0.0, $GNRS_listen_port) 

//queue to hold responses from GNRS service
gnrs_respQ::Queue(100);

gnrs_reqQ -> Unqueue -> [0]gnrs_rrh;
//send requests to service
gnrs_rrh[0] -> gnrs_svc_sndr;
//recv & queue responses for processing & forwarding to requestors
gnrs_svc_lstnr -> gnrs_respQ -> Unqueue -> [1]gnrs_rrh;

//Requestor 1: Host association handler
//successful host associations result in GNRS updates
assoc_hndlr -> gnrs_reqQ;
//TODO: patch responses to updates back to assoc handler

//Requestor 2: Network binder
//Patch GNRS lookup requests/response from/to GNRS service client
net_binder[1] -> gnrs_reqQ;
gnrs_rrh[1] -> [1]net_binder;

//Thread/Task Scheduling
//re-balance tasks to threads every 10ms
//BalancedThreadSched(INTERVAL 10);
StaticThreadSched(fd_core 1, td_core 1, rtg_tbl 3);
//StaticThreadSched(fd_core 0, vchk_src 0, inUnq 1, nbUnq 1, segUnq 1, swUnq 1, td_core 2, seg 2, lp_hndlr 3, lsa_hndlr 3, rtg_tbl 3, g1Unq 3, g2Unq 3, gnrs_svc_sndr 3, gnrs_svc_lstnr 3);
