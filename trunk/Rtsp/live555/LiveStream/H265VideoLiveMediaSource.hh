#ifndef _H265_VIDEO_LIVE_MEDIA_SOURCE_HH
#define _H265_VIDEO_LIVE_MEDIA_SOURCE_HH

#include <MediaSink.hh>
typedef void (*callback_func)(void *ptr,int streamid);
#define DEFAULT_MAX_VEDIOFRAME_RATE 30
#define DEFAULT_MAX_VEDIOFRAME_SIZE 64 * 1024

class H265VideoLiveMediaSource:public Medium
{
public:
	
	~H265VideoLiveMediaSource();
	static H265VideoLiveMediaSource* createNew(UsageEnvironment& env);

	FramedSource* videoSource();
	void VideoPointerSync();	//sync read && write pointer and demand IDR
	static void SetVideoPointerSyncCallback(callback_func handler,void *ptr);
	static void SetVideoDemandStreamStartCallback(callback_func handler,void *ptr);
	
private:
	H265VideoLiveMediaSource(UsageEnvironment& env);
public:
	static callback_func funDemandIDR;
	static void * callback_ptr;
	
    static callback_func funDemandStreamStart;
    static void * callback_ptr_stram;
	static bool    m_bVideoStart;
	static FramedSource* fOurVideoSource;
};


#ifndef _FRAMED_FILE_SOURCE_HH
#include "FramedSource.hh"
#endif

class VideoOpenSource: public FramedSource
{ 
public:
  static VideoOpenSource* createNew(UsageEnvironment& env,
					 char const* fileName,
					 H265VideoLiveMediaSource& input,
					 unsigned preferredFrameSize = 0,
					 unsigned playTimePerFrame = 0);
  // "preferredFrameSize" == 0 means 'no preference'
  // "playTimePerFrame" is in microseconds

  unsigned maxFrameSize() const {
    return DEFAULT_MAX_VEDIOFRAME_SIZE;
  }

  void SyncWRPointer();

protected:
  VideoOpenSource(UsageEnvironment& env,
		       H265VideoLiveMediaSource& input,
		       unsigned preferredFrameSize,
		       unsigned playTimePerFrame);

  virtual ~VideoOpenSource();

  static void incomingDataHandler(VideoOpenSource* source);
  void incomingDataHandler1();

private:
  // redefined virtual functions:
  virtual void doGetNextFrame();

protected:
  H265VideoLiveMediaSource& fInput;

private:
  unsigned fPreferredFrameSize;
  unsigned fPlayTimePerFrame;
  Boolean fFidIsSeekable;
  unsigned fLastPlayTime;
  Boolean fHaveStartedReading;
  Boolean fLimitNumBytesToStream;
  u_int64_t fNumBytesToStream; // used iff "fLimitNumBytesToStream" is True

  double m_FrameRate;
  int  uSecsToDelay;
  int  uSecsToDelayMax;
};


#endif
