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
/*
 * MF_Aggregator.hh
 *
 *  Created on: Jun 2, 2011
 *      Author: Kai Su
 */

#ifndef MF_AGGREGATOR_HH_
#define MF_AGGREGATOR_HH_

#include <click/element.hh>
#include <click/task.hh>
#include <click/vector.hh>
#include <click/hashtable.hh>

#include "mfarptable.hh"
#include "mfrouterstats.hh"
#include "mfchunkmanager.hh"
#include "mf.hh"
#include "mfchunk.hh"
#include "mflogger.hh"

CLICK_DECLS
class HandlerCall;

// key is string for mac address and value is <sources' hop ID, local hop ID> tuple.
//<src_id, <chunk_id, chunk*>>
typedef HashTable<uint32_t, HashTable<uint32_t, Chunk*>* > HopIDMap;

/**
 * In Ports: 0 - Data packet; 1 - CSYN packet
 * Out Ports: 0 - Aggregated chunk packet; 1 - CSYN-ACK packet
 */
class MF_Aggregator : public Element {
 public:
  MF_Aggregator();
  ~MF_Aggregator();
  const char *class_name() const		{ return "MF_Aggregator"; }
  const char *port_count() const		{ return "2/2-3"; }
  const char *processing() const		{ return "h/h"; }

  int configure(Vector<String>&, ErrorHandler *);
  void push(int port, Packet *p);
	
  void add_handlers();

 private:
  void handleCSYNPkt (Packet *);
  void handleDataPkt (Packet *); 
  static String read_handler(Element *e, void *thunk);
	
  HopIDMap *_hopIDMap;

  int _timeToAck;
  int _recvd_chk_count;
  
  uint32_t _myGUID; 
  
  MF_ChunkManager *_chunkManager;
  MF_RouterStats *_routerStats; 
  MF_Logger logger;
};


CLICK_ENDDECLS
#endif /* MF_AGGREGATOR_HH_ */
