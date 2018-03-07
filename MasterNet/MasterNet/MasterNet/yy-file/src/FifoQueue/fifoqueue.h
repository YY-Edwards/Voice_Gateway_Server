/*
 * FifoQueue.h
 *
 * Created: 2018/01/03
 * Author: EDWARDS
 */ 

#ifndef fifoqueue_h_
#define fifoqueue_h_
#include "..\Common\common.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <list>
#include <queue>
#include <stdint.h>
#include "..\SynInterface\syninterface.h"

#define FIFODEEP  40
#pragma pack(push, 1)
typedef struct{
	char		data[512];
	int			len;

}fifoqueue_t;

#pragma pack(pop)




class FifoQueue 
{
	
	public:
		FifoQueue();
		~FifoQueue();

	public:
		bool  			PushToQueue(void *packet, int len);
		int32_t 		TakeFromQueue(void *packet, int& len, int waittime =200);
		void			ClearQueue();
		bool 			QueueIsEmpty();

	private:

		std::list<fifoqueue_t *>  	m_list;
		fifoqueue_t fifobuff[FIFODEEP];
		volatile  uint32_t fifo_counter;
		ILock	*queuelock;
		ISem	*queuesem;

	
};



#endif