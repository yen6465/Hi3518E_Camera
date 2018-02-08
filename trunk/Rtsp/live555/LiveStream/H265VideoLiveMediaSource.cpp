#include "H265VideoLiveMediaSource.hh"
#include "MPEGVideoStreamParser.hh"
#include "CircularQueue.h"
#include "debuginfo.h"

callback_func H265VideoLiveMediaSource::funDemandIDR = NULL;
void * H265VideoLiveMediaSource::callback_ptr = NULL;

callback_func H265VideoLiveMediaSource::funDemandStreamStart = NULL;
void * H265VideoLiveMediaSource::callback_ptr_stram = NULL;

bool H265VideoLiveMediaSource::m_bVideoStart = false;
FramedSource* H265VideoLiveMediaSource::fOurVideoSource = NULL;

extern pthread_mutex_t m_bVideoStartRW;

H265VideoLiveMediaSource::~H265VideoLiveMediaSource()
{
	DEBUG_ERROR("VideoOpenSource::~VideoOpenSource()");
	if(fOurVideoSource != NULL)
	{
	    DEBUG_ERROR("H265VideoLiveMediaSource::~H265VideoLiveMediaSource");
	    Medium::close(fOurVideoSource);
		fOurVideoSource = NULL;
	}
}
H265VideoLiveMediaSource* H265VideoLiveMediaSource::createNew(UsageEnvironment& env)
{
	return new H265VideoLiveMediaSource(env);
}

void H265VideoLiveMediaSource::VideoPointerSync()
{
	if(funDemandIDR!=NULL)//pointer not null
	{
		funDemandIDR(callback_ptr,0);
	}
}

void H265VideoLiveMediaSource::SetVideoPointerSyncCallback(callback_func handler,void *ptr)
{
	funDemandIDR = handler;
	callback_ptr = ptr;
}

void H265VideoLiveMediaSource::SetVideoDemandStreamStartCallback(callback_func handler,void *ptr)
{
	funDemandStreamStart = handler;
	callback_ptr_stram = ptr;
}


FramedSource* H265VideoLiveMediaSource::videoSource()
{
	DEBUG_ERROR("=======================>>>>>get videoSource m_bVideoStart:%d<<<==================",m_bVideoStart);
//	if(!m_bVideoStart)
//	{
//		return NULL;
//	}

	//DEMAND IDR
	CCircularQueue::getInstance()->SyncRwPoint();
	VideoPointerSync();
	
	if(fOurVideoSource == NULL)
	{
		DEBUG_ERROR("fOurVideoSource == NULL");
		fOurVideoSource = VideoOpenSource::createNew(envir(), NULL, *this);
		
		DEBUG_ERROR("===fOurVideoSource:%x====",fOurVideoSource);
	}

	return fOurVideoSource;
}

H265VideoLiveMediaSource::H265VideoLiveMediaSource(UsageEnvironment& env)
	: Medium(env)
{

}

static int PopFrontErrCount = 0;
static int g_fwdFrameDataLen = 0;

VideoOpenSource*
VideoOpenSource::createNew(UsageEnvironment& env, char const* fileName,H265VideoLiveMediaSource& input,
				unsigned preferredFrameSize,
				unsigned playTimePerFrame) {
				
 	VideoOpenSource* newSource = new VideoOpenSource(env, input, preferredFrameSize, playTimePerFrame);

  	return newSource;
}

void VideoOpenSource::SyncWRPointer()
{
	CCircularQueue::getInstance()->SyncRwPoint();
}


VideoOpenSource::VideoOpenSource(UsageEnvironment& env,
						H265VideoLiveMediaSource& input,
					   unsigned preferredFrameSize,
					   unsigned playTimePerFrame)
  : FramedSource(env), fInput(input),fPreferredFrameSize(preferredFrameSize),
    fPlayTimePerFrame(playTimePerFrame), fLastPlayTime(0),
    fHaveStartedReading(False), fLimitNumBytesToStream(False), fNumBytesToStream(0)
    ,m_FrameRate(DEFAULT_MAX_VEDIOFRAME_RATE){
    
    uSecsToDelay = 1000;
 	uSecsToDelayMax = 1666;
}

VideoOpenSource::~VideoOpenSource() 
{
	DEBUG_ERROR("VideoOpenSource::~VideoOpenSource()");
	if(fInput.fOurVideoSource != NULL)
	{
        Medium::close(fInput.fOurVideoSource);
	    fInput.fOurVideoSource = NULL;
	}
}

void VideoOpenSource::doGetNextFrame() {
	//do read from memory
	incomingDataHandler(this);
}
void VideoOpenSource::incomingDataHandler(VideoOpenSource* source) {
	source->incomingDataHandler1();
}

//**********************************************************************//
FILE *fp_h265 = NULL;
int read_h265(unsigned char *buf, int buf_size){
    if(fp_h265 == NULL)
    {
        fp_h265 = fopen("test.265", "rb");
    }
    
	if (!feof(fp_h265)){
		int true_size = fread(buf, 1, buf_size, fp_h265);
		return true_size;
	}
	else{
		fseek(fp_h265, 0L, SEEK_SET);
		return 0;
	}
}
//**********************************************************************//

void VideoOpenSource::incomingDataHandler1()
{
	if (fLimitNumBytesToStream && fNumBytesToStream < (u_int64_t)fMaxSize) {
        fMaxSize = (unsigned)fNumBytesToStream;
    }
    if (fPreferredFrameSize > 0 && fPreferredFrameSize < fMaxSize) {
        fMaxSize = fPreferredFrameSize;
    }
    
 	fFrameSize = 0;
 	Queue_Element elem;
	int unHandleCnt = 0;

    //add for file simulation test
    if(CCircularQueue::getInstance()->Size() < 2)
    {
        //read file , push 
        Queue_Element pushElem;
        int rdSize = read_h265(pushElem.data,64*1024);
        pushElem.lenght = rdSize;
        pushElem.FrameCompleted = 1;
        CCircularQueue::getInstance()->PushBack(pushElem);
    }
	
	if(CCircularQueue::getInstance()->PopFront(&elem,unHandleCnt))
	{
	    //DEBUG_DEBUG("====>>>data:%x elem.lenght:%d",elem.data,elem.lenght);
		fFrameSize = elem.lenght;
		PopFrontErrCount = 0;
	}
	else
	{
        PopFrontErrCount++;
        if(PopFrontErrCount > 500)
        {
            DEBUG_DEBUG("Error ====>>>data:%x elem.lenght:%d",elem.data,elem.lenght);
            PopFrontErrCount = 0;
        }
	}
    
 	if(fFrameSize < 0)
 	{
		handleClosure(this);
		return;
    }
    else if (fFrameSize == 0)
    {
		if( uSecsToDelay >= uSecsToDelayMax )
		{
			uSecsToDelay = uSecsToDelayMax;
		}else{
			uSecsToDelay *= 2;
		}
		
		if(PopFrontErrCount%10 == 0)
		{
			fNumTruncatedBytes = 0;
			gettimeofday(&fPresentationTime, NULL);
			//DEBUG_ERROR("No data for long time, call FramedSource::afterGetting");
			nextTask() = envir().taskScheduler().scheduleDelayedTask(0,
				(TaskFunc*)FramedSource::afterGetting, this);
		}
		else
		{
			nextTask() = envir().taskScheduler().scheduleDelayedTask(uSecsToDelay,
			      (TaskFunc*)incomingDataHandler, this);
		}
	}
	else 
	{
		if (fFrameSize + g_fwdFrameDataLen > fMaxSize) 
		{
		    DEBUG_ERROR("----->>>>fFrameSize > fMaxSize lost data!!!");
			fNumTruncatedBytes = fFrameSize + g_fwdFrameDataLen - fMaxSize;
			fFrameSize = fMaxSize - g_fwdFrameDataLen;
		}
		else
		{
			fNumTruncatedBytes = 0;
		}

        
		if(elem.FrameCompleted == 0)
		{
            memcpy(fTo, elem.data, fFrameSize);
            g_fwdFrameDataLen += fFrameSize;

            nextTask() = envir().taskScheduler().scheduleDelayedTask(uSecsToDelay,
			      (TaskFunc*)incomingDataHandler, this);
		}
        else
        {
            memcpy(fTo + g_fwdFrameDataLen, elem.data, fFrameSize);
            g_fwdFrameDataLen = 0;
            gettimeofday(&fPresentationTime, NULL);
		
            nextTask() = envir().taskScheduler().scheduleDelayedTask(0,
				(TaskFunc*)FramedSource::afterGetting, this);
        }
	}
}
