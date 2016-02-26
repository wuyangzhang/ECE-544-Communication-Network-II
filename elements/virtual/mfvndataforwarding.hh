/*
 *  Copyright (c) 2010-2013, Rutgers, The State University of New Jersey
 *  All rights reserved.
 *  
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *      * Redistributions of source code must retain the above copyright
 *        notice, this list of conditions and the following disclaimer.
 *      * Redistributions in binary form must reproduce the above copyright
 *        notice, this list of conditions and the following disclaimer in the
 *        documentation and/or other materials provided with the distribution.
 *      * Neither the name of the organization(s) stated above nor the
 *        names of its contributors may be used to endorse or promote products
 *        derived from this software without specific prior written permission.
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 *  ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef MF_VN_DATA_FORWARDING_HH
#define MF_VN_DATA_FORWARDING_HH
#define DEFAULT_CHK_SIZE 7000
#define NODE_METRIC_THRESHOLD 0.5
#define MAX_TIME_VALUE_MSECS 100000

#include <click/element.hh>
#include <click/timer.hh>
#include "mf.hh"
#include "mflogger.hh"
#include "mfvnroutingtable.hh"
#include "mfvirtualctrlmsg.hh"
#include "mfchunkmanager.hh"
#include "mfvnmanager.hh"
#include "mfvninfocontainer.hh"


/*!
  Multiplication factor for ASP node metric at the server end 
  Waiting_time * MAX_VALUE_FTT : done at the server 
 */

//TODO refactoring: move to use new vn container.
#define MAX_VALUE_FTT 1000

CLICK_DECLS

class MF_VNDataForwarding: public Element{
public:
	MF_VNDataForwarding();
	~MF_VNDataForwarding();

	const char *class_name() const	{ return "MF_VNDataForwarding"; }
	const char *port_count() const	{ return "1/1"; }
	const char *processing() const	{ return PUSH; }

	void push (int, Packet *);
	int configure(Vector<String> &, ErrorHandler *);
	//  int initialize(ErrorHandler *);
	//  void run_timer(Timer *);

private:
	uint32_t _service_id;
	//  uint32_t _curr_chk_ID;
	uint32_t _pkt_size;
	uint32_t _chk_size;


	MF_ChunkManager *_chunkManager;
	MF_VNManager * _vnManager;


	/* functions */
	void forwardVirtualData(Packet *);
	uint32_t findBestDestination(MF_VNInfoContainer *container, uint32_t);
	uint32_t findBestDestination_algo2(MF_VNInfoContainer *container, uint32_t);

	int algorithm_indicator;

	MF_Logger logger;
};

CLICK_ENDDECLS;
#endif
