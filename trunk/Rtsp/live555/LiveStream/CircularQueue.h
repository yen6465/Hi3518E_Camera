/********************************************************************
filename: 	CircularQueue.h
created:	2016-01-13
author:		donyj

purpose:
            
			

!!!important:	
*********************************************************************/
#pragma once
#include <pthread.h>

#define DEFAULT_MAX_FRAME_NUM	5
#define DEFAULT_MAX_FRAME_SIZE	64*1024

struct QUEUE_ELEMENT_STU
{
	unsigned int lenght;
#ifdef POINT_MODE
	unsigned char* data;
#else
	unsigned char data[DEFAULT_MAX_FRAME_SIZE];
#endif
	unsigned char FrameCompleted;
};
typedef struct QUEUE_ELEMENT_STU Queue_Element;

class CCircularQueue
{
public:
	
	CCircularQueue();
	CCircularQueue(int nQueueSize);
	~CCircularQueue(void);
public:
	bool PushBack(const Queue_Element elem);
	
	bool PopFront(Queue_Element *pElem, int &unHandleCnt);

	void SyncRwPoint();
		
	int Size();
	int RemainSize();
	bool IsEmpty();
	bool IsFull();
	static CCircularQueue* getInstance();
	static CCircularQueue* m_pInstance;
private:
	Queue_Element *m_pQueueBase;
	int m_QueueSize;            
	int m_Head;
	int m_Tail;         
	pthread_mutex_t m_csRW;

};

