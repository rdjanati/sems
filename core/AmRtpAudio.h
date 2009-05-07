/*
 * $Id$
 *
 * Copyright (C) 2002-2003 Fhg Fokus
 *
 * This file is part of sems, a free SIP media server.
 *
 * sems is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version
 *
 * For a license to use the ser software under conditions
 * other than those described here, or to purchase support for this
 * software, please contact iptel.org by e-mail at the following addresses:
 *    info@iptel.org
 *
 * sems is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, write to the Free Software 
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
/** @file AmRtpAudio.h */
#ifndef _AmRtpAudio_h_
#define _AmRtpAudio_h_

#include "AmAudio.h"
#include "AmRtpStream.h"
#include "LowcFE.h"

#ifdef USE_SPANDSP_PLC
#include <math.h>
#include "spandsp/plc.h"
#endif

class AmPlayoutBuffer;

enum PlayoutType {
  ADAPTIVE_PLAYOUT,
  JB_PLAYOUT,
  SIMPLE_PLAYOUT
};

/** 
 * \brief interface for PLC buffer
 */

class AmPLCBuffer {
 public: 

  virtual void add_to_history(int16_t *buffer, unsigned int size) = 0;

  // Conceals packet loss into the out_buffer
  // @return length in bytes of the recivered segment
  virtual unsigned int conceal_loss(unsigned int ts_diff, unsigned char *out_buffer) = 0;
  AmPLCBuffer() { }
  virtual ~AmPLCBuffer() { }
};

/** \brief RTP audio format */
class AmAudioRtpFormat: public AmAudioFormat
{
  vector<SdpPayload *> m_payloads;
  int m_currentPayload;
  amci_payload_t *m_currentPayloadP;
  std::map<int, SdpPayload *> m_sdpPayloadByPayload;
  std::map<int, amci_payload_t *> m_payloadPByPayload;
  std::map<int, CodecContainer *> m_codecContainerByPayload;

protected:
  virtual int getCodecId();

public:
  /**
   * Constructor for payload based formats.
   * All the information are taken from the 
   * payload description in the originating plug-in.
   */
  AmAudioRtpFormat(const vector<SdpPayload *>& payloads);
  ~AmAudioRtpFormat();

  /**
   * changes payload. returns != 0 on error.
   */
  int setCurrentPayload(int payload);

  int getCurrentPayload() { return m_currentPayload; };
};

/** 
 * \brief binds together a \ref AmRtpStream and an \ref AmAudio for a session 
 */
class AmRtpAudio: public AmRtpStream, public AmAudio, public AmPLCBuffer
{
  auto_ptr<AmPlayoutBuffer> playout_buffer;

#ifdef USE_SPANDSP_PLC
    plc_state_t* plc_state;
#else 
    LowcFE       fec;
#endif

  bool         use_default_plc;

  unsigned int last_check;
  bool         last_check_i;
  bool         send_int;

  bool         send_only;

  //
  // Default packet loss concealment functions
  //
  unsigned int default_plc(unsigned char* out_buf,
			   unsigned int   size,
			   unsigned int   channels,
			   unsigned int   rate);

public:
  AmRtpAudio(AmSession* _s=0);
  ~AmRtpAudio();

  bool checkInterval(unsigned int ts);
  bool sendIntReached();

  int setCurrentPayload(int payload);
  int getCurrentPayload();

  int receive(unsigned int wallclock_ts);

  void setSendOnly(bool so){
    send_only = so;
  }

  // AmAudio interface
  int read(unsigned int user_ts, unsigned int size);
  int write(unsigned int user_ts, unsigned int size);

  int get(unsigned int user_ts, unsigned char* buffer, 
	  unsigned int time_millisec);

  // AmRtpStream interface
  void init(const vector<SdpPayload*>& sdp_payloads);

  void setPlayoutType(PlayoutType type);

  virtual unsigned int bytes2samples(unsigned int) const;

  void add_to_history(int16_t *buffer, unsigned int size);

  // Conceals packet loss into the out_buffer
  // @return length in bytes of the recivered segment
  unsigned int conceal_loss(unsigned int ts_diff, unsigned char *out_buffer);

  /** get frame length in samples */
  unsigned int getFrameSize();

  /** get frame length in ms */
  unsigned int getFrameLength();
};

#endif






