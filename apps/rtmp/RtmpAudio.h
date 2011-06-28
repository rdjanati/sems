#ifndef _RtmpAudio_h_
#define _RtmpAudio_h_

#include "AmAudio.h"
#include "AmPlayoutBuffer.h"
#include "AmRtpAudio.h"

#include "librtmp/rtmp.h"
#include <speex/speex.h>

#include <queue>
using std::queue;

class RtmpSender;

class RtmpAudio
  : public AmAudio,
    public AmPLCBuffer
{
  struct SpeexState {
    void *state;
    SpeexBits bits;
  };

  RtmpSender* sender;
  AmMutex     m_sender;

  queue<RTMPPacket> q_recv;
  AmMutex           m_q_recv;

  AmAdaptivePlayout  playout_buffer;
  unsigned int       play_stream_id;

  bool         recv_offset_i;
  unsigned int recv_rtp_offset;
  unsigned int recv_rtmp_offset;

  bool         send_offset_i;
  unsigned int send_rtmp_offset;

  SpeexState dec_state;
  SpeexState enc_state;

  void init_codec();
  int wb_decode(unsigned int size);
  int wb_encode(unsigned int size);

  void process_recv_queue(unsigned int ref_ts);

public:
  RtmpAudio(RtmpSender* s, unsigned int play_stream_id);
  ~RtmpAudio();

  /* @see AmAudio */
  int get(unsigned int user_ts, unsigned char* buffer, unsigned int nb_samples);
  int put(unsigned int user_ts, unsigned char* buffer, unsigned int size);
  int read(unsigned int user_ts, unsigned int size);
  int write(unsigned int user_ts, unsigned int size);

  void bufferPacket(const RTMPPacket& p);

  /* @see AmPLCBuffer */
  void add_to_history(int16_t *, unsigned int);
  unsigned int conceal_loss(unsigned int, unsigned char *);

  /* 
   * Called by RtmpSession when 
   * the connection has been released 
   * or changed.
   */
  void setSenderPtr(RtmpSender* s);
};

#endif