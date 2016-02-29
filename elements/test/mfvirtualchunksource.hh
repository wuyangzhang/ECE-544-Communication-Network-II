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
 * MF_ChunkSource.hh
 *
 */

#ifndef MF_VIRTUAL_CHUNKSOURCE_HH_
#define MF_VIRTUAL_CHUNKSOURCE_HH_

#include <click/element.hh>
#include <click/task.hh>
#include <click/notifier.hh>
#include <click/vector.hh>
#include <click/standard/scheduleinfo.hh>

#include "mf.hh"
#include "mfrouterstats.hh"
#include "mfchunkmanager.hh"
#include "mflogger.hh"
#include "mfvirtualextheader.hh"

class MF_VirtualChunkSource : public Element {
 public:
  MF_VirtualChunkSource();
  ~MF_VirtualChunkSource();

  const char *class_name() const		{ return "MF_VirtualChunkSource"; }
  const char *port_count() const		{ return "0/1"; }
  const char *processing() const		{ return "h/h"; }

  int configure(Vector<String>&, ErrorHandler *);
  int initialize(ErrorHandler *);
//  void add_handlers();

  bool run_task(Task *);

 private:
//  bool sendStartMeasurementMsg(); 
  
  Task _task;
  Timer _timer;
  bool _active;
  bool _start_msg_sent; 
  /* to check if downstream has capacity */
  NotifierSignal _signal;

  /* supplied arguments */
  uint32_t _src_GUID;
  uint32_t _dst_GUID;
  uint32_t _virtual_network_GUID;
  uint32_t _virtual_src_GUID;
  uint32_t _virtual_dst_GUID;
//  uint32_t _service_id;
  uint32_t _chk_size; //bytes
//  int32_t _chk_lmt;
  uint32_t _chk_intval_msec;
  uint32_t _delay_sec;
  uint32_t _pkt_size;
  String _dst_NAs_str;
  
  /* monotonically increasing unique chunk identifier, starts at 1 */
  uint32_t _curr_chk_ID;
  uint32_t _curr_chk_cnt;
  Vector<NA> * _dst_NAs; 
  
  MF_RouterStats *_routerStats;               
  MF_ChunkManager *_chunkManager; 
  MF_Logger logger;
};

CLICK_ENDDECLS
#endif /* MF_CHUNKSOURCE_HH_ */
