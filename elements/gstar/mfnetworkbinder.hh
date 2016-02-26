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
 * MF_NetworkBinder.hh
 *
 *  Created on: Jul 16, 2011
 *      Author: Kai Su
 */

#ifndef MF_NETWORKBINDER_HH_
#define MF_NETWORKBINDER_HH_

#include <click/element.hh>
#include <click/hashtable.hh>

#include "mf.hh"
#include "mflogger.hh"
#include "mfchunkmanager.hh"
#include "gnrsrespcache.hh"
#include "mfroutingtable.hh"
CLICK_DECLS

class MF_NetworkBinder : public Element {
public:
  MF_NetworkBinder();
  ~MF_NetworkBinder();
  
  const char *class_name() const		{ return "MF_NetworkBinder"; }
  const char *port_count() const		{ return "2/2"; }
  const char *processing() const		{ return "h/h"; }

  int configure(Vector<String>&, ErrorHandler *);
  void push(int port, Packet *p);

private:
  void handleGNRSResp(Packet *);
  void sendGNRSReqst(GUID dst_GUID); /* dst_GUID should be in host byte order */ 
  void handleChunkPkt(Packet *);
  
  uint32_t _myGUID;  

  HashTable<uint32_t, Packet*> *_resolvingTable; 
  MF_ChunkManager *_chunkManager;
  GNRS_RespCache *_respCache;
  MF_Logger logger;
};

CLICK_ENDDECLS

#endif /* MF_NETWORKBINDER_HH_ */
