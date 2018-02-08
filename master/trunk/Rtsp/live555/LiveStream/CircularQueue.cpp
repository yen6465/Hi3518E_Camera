/********************************************************************
filename: 	CircularQueue.cpp
created:	2016-01-13
author:	    donyj

purpose:	
            
			
*********************************************************************/
#include "CircularQueue.h"
#include <string.h>
#include <stdio.h>
#include "debuginfo.h"

CCircularQueue* CCircularQueue::m_pInstance = NULL;
CCircularQueue* CCircularQueue::getInstance()
{
	if(m_pInstance == NULL)
	{
		m_pInstance = new CCircularQueue();
	}
	return m_pInstance;
}

CCircularQueue::CCircularQueue():
m_pQueueBase(NULL),
m_QueueSize(DEFAULT_MAX_FRAME_NUM),
m_Head(0),
m_Tail(0)

{
	m_pQueueBase = new Queue_Element[m_QueueSize];
	pthread_mutex_init(&m_csRW,NULL);
}

CCircularQueue::CCircularQueue(int nQueueSize):
m_pQueueBase(NULL),
m_QueueSize(nQueueSize),
m_Head(0),
m_Tail(0)
{
	m_pQueueBase = new Queue_Element[m_QueueSize];
	pthread_mutex_init(&m_csRW,NULL);
}

CCircularQueue::~CCircularQueue(void)
{
	if(m_pQueueBase)
	{
		delete[] m_pQueueBase;
		m_pQueueBase = NULL;
	}
	pthread_mutex_destroy(&m_csRW);
}

bool CCircularQueue::PushBack(const Queue_Element elem)
{
	if(m_pQueueBase == NULL || elem.data == NULL || elem.lenght <= 0)
	{
		return false;
	}
#ifndef POINT_MODE
    if(elem.lenght > DEFAULT_MAX_FRAME_SIZE)
    {
        return false;
    }
#endif
	//DEBUG_ERROR("--->>>elem.data:%x,elem.lenght:%d ",elem.data,elem.lenght);
	pthread_mutex_lock(&m_csRW);

	if(IsFull())
	{
		//printf("Warning:lost data, Queue is full..\n");
		if(m_pQueueBase[m_Head].FrameCompleted != 1)
		{
			m_Head = (m_Head+2)% m_QueueSize;
		}
		else
		{
			m_Head = (m_Head+1)% m_QueueSize;
		}
		
	}

#ifdef POINT_MODE
	m_pQueueBase[m_Tail].data = elem.data;
#else
    memcpy(m_pQueueBase[m_Tail].data, elem.data, elem.lenght);
#endif
	m_pQueueBase[m_Tail].lenght = elem.lenght;
	m_pQueueBase[m_Tail].FrameCompleted = elem.FrameCompleted;

	m_Tail = (m_Tail+1) % m_QueueSize;
	
	pthread_mutex_unlock(&m_csRW);
	return true;
}


bool CCircularQueue::PopFront(Queue_Element *pElem, int &unHandleCnt)
{
	pthread_mutex_lock(&m_csRW);
	if(IsEmpty())
	{
		//printf("Warning: Queue is empty..\n");
		unHandleCnt = 0;
		//pElem->data = NULL;
		pElem->lenght = 0;
		pElem->FrameCompleted = 1;
		pthread_mutex_unlock(&m_csRW);
		return false;
	}

#ifdef POINT_MODE
	pElem->data = m_pQueueBase[m_Head].data;
#else
    memcpy(pElem->data, m_pQueueBase[m_Head].data, m_pQueueBase[m_Head].lenght);
#endif
	
	pElem->lenght = m_pQueueBase[m_Head].lenght;
	pElem->FrameCompleted = m_pQueueBase[m_Head].FrameCompleted;

	m_Head = (m_Head+1) % m_QueueSize;
	unHandleCnt = (m_Tail - m_Head + m_QueueSize) % m_QueueSize;
	//DEBUG_ERROR("<<<---elem.data:%x,elem.lenght:%d size:%d FrameCompleted:%d",pElem->data,pElem->lenght,(m_Tail - m_Head + m_QueueSize) % m_QueueSize,pElem->FrameCompleted);
	pthread_mutex_unlock(&m_csRW);
	
	return true;
}

void CCircularQueue::SyncRwPoint()
{
    pthread_mutex_lock(&m_csRW);
    m_Head = m_Tail;    
    pthread_mutex_unlock(&m_csRW);
	DEBUG_ERROR("CCircularQueue::SyncRwPoint");
}


int CCircularQueue::Size()
{
	int size = 0;
	
	pthread_mutex_lock(&m_csRW);
	size = (m_Tail - m_Head + m_QueueSize) % m_QueueSize;
	pthread_mutex_unlock(&m_csRW);
	
	return size;
}


bool CCircularQueue::IsEmpty()
{
	return (m_Head == m_Tail) ? true : false;
}


bool CCircularQueue::IsFull()
{
	return (((m_Tail+1) % m_QueueSize) == m_Head) ? true : false;
}


int CCircularQueue::RemainSize()
{
	return ((m_QueueSize)-Size());
}
