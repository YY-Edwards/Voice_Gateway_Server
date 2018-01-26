/*
 * FifoQueue.c
 *
 * Created: 2016/12/19
 * Author: EDWARDS
 */ 
#include "stdafx.h"
#include "fifoqueue.h"

FifoQueue::FifoQueue()
:queuelock(NULL)
, queuesem(NULL)
,fifo_counter(0)
{

#ifdef WIN32
	queuelock = new CriSection();
#else
	queuelock = new Mutex((const char *)"queuelocker");
#endif

	queuesem = new MySynSem();
	for (int i = 0; i < FIFODEEP; i++){
		memset(&(fifobuff[i].data), 0x00, 512);
		memset(&(fifobuff[i].len), 0x00, sizeof(uint8_t));
		//memset(&fifobuff[i][0], 0x00, 512);
	}

}
FifoQueue::~FifoQueue()
{
	if (queuelock != NULL)
	{
		delete queuelock;
		queuelock = NULL;
	}
	if (queuesem != NULL)
	{
		delete queuesem;
		queuesem = NULL;
	}
	
}

void FifoQueue::ClearQueue()
{
	queuelock->Lock();
	m_list.clear();
	queuelock->Unlock();

}


bool  FifoQueue::QueueIsEmpty()
{
	return(m_list.empty());
}
bool FifoQueue::PushToQueue(void *packet, int len)
{
	int err = 0;
	if (m_list.size() >= FIFODEEP)return false;//fifo full

	queuelock->Lock();
	////清空结构体
	//memset(&(fifobuff[fifo_counter].data), 0x00, 512);
	//memset(&(fifobuff[fifo_counter].len), 0x00, sizeof(uint8_t));
	//memcpy(&fifobuff[fifo_counter][0], packet, len);
	//m_list.push_back(&fifobuff[fifo_counter][0]);//将一个数据包地址插入链表尾

	memcpy(&fifobuff[fifo_counter].data, packet, len);
	fifobuff[fifo_counter].len = len;
	m_list.push_back(&fifobuff[fifo_counter]);//将一个结构体数据地址插入链表尾
	fifo_counter++;
	if (fifo_counter > 19){
		fifo_counter = 0;
	}

	queuelock->Unlock();

	queuesem->SemPost();

	return true;

}

int32_t FifoQueue::TakeFromQueue(void *packet, int& len, int waittime)
{

	//char* sBuffer = NULL;
	fifoqueue_t *sBuffer = NULL;

	int ret = queuesem->SemWait(waittime);
	if ((ret < 0) || (ret >= 1))
	{
		return ret;//timeout or failed
	}
	if (!m_list.empty())
	{
			queuelock->Lock();

			sBuffer = m_list.front();//返回链表第一个结构体数据包地址给sbuffer
			m_list.pop_front();//并删除链表第一个数据包地址
			memcpy(packet, sBuffer->data, sBuffer->len);
			len = sBuffer->len;

			queuelock->Unlock();
			return ret;
	}
	else
	{
		return -2;//no happen but must check

	}


}



