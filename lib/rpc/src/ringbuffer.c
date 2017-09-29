#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "..\include\ringbuffer.h"

pRingBuffer createRingBuffer(int bufSize, int elSize)
{
	if (0 == bufSize || 0 == elSize)
	{
		goto err;
	}

	pRingBuffer pBuffer = (pRingBuffer)malloc(sizeof(tRingBuffer));
	if (pBuffer)
	{
		pBuffer->bufferSize = bufSize;
		pBuffer->elementSize = elSize;
		pBuffer->empty = BufferEmptyYes;
		pBuffer->head = 0;
		pBuffer->tail = 0;
		pBuffer->pData = (unsigned char*)malloc(bufSize * elSize);

		if (0 == pBuffer->pData)
		{
			free(pBuffer);
			goto err;
		}

		return pBuffer;
	}

err:
	return 0;
}

void freeRingBuffer(pRingBuffer pBuff)
{
	if (NULL == pBuff)
	{
		return;
	}

	free(pBuff->pData);
	free(pBuff);
}

bool isEmpty(const pRingBuffer pBuff)
{
	if (NULL == pBuff)
	{
		return true;
	}

	return (BufferEmptyYes == pBuff->empty);
}

bool isFull(const pRingBuffer pBuff)
{
	if (NULL == pBuff)
	{
		return false;
	}

	return (pBuff->head == pBuff->tail &&
		BufferEmptyNo == pBuff->empty);
}

int getRingBufferSize(const pRingBuffer pBuff)
{
	int n = 0;
	if (NULL == pBuff)
	{
		goto err;
	}


	if (pBuff->head >= pBuff->tail)
	{
		n = pBuff->head - pBuff->tail;
	}
	else
	{
		n = pBuff->bufferSize + pBuff->head - pBuff->tail;
	}

err:
	return n;
}

int push(pRingBuffer pBuff, const void* pEl)
{
	if (NULL == pEl || NULL == pBuff)
	{
		goto err;
	}

	unsigned char* pByte = (unsigned char*)pEl;
	if (isFull(pBuff))
	{
		// buffer is full
		goto err;
	}
	
	int position = pBuff->head;
	memcpy((pBuff->pData + position * pBuff->elementSize), pByte, pBuff->elementSize);
	
	pBuff->head++;
	if (pBuff->head >= pBuff->bufferSize)
	{
		pBuff->head = 0;
	}

	pBuff->empty = BufferEmptyNo;

	return position;
err:
	return -1;
}

int pop(pRingBuffer pBuff, void* pEl)
{
	if (NULL == pBuff || NULL == pEl)
	{
		goto err;
	}

	if (BufferEmptyYes == pBuff->empty)
	{
		goto err;
	}

	// copy value out
	memcpy(pEl, pBuff->pData + pBuff->tail * pBuff->elementSize, pBuff->elementSize);
	
	pBuff->tail++;
	if (pBuff->tail >= pBuff->bufferSize)
	{
		pBuff->tail = 0;
	}

	if (pBuff->tail == pBuff->head)
	{
		pBuff->empty = BufferEmptyYes;
	} 

	return 1;

err:
	return 0;
}

int capacity(const pRingBuffer pBuff)
{
	int n = 0;
	if (NULL == pBuff)
	{
		goto err;
	}

	return pBuff->bufferSize;

err:
	return n;
}

int getFreeSize(const pRingBuffer pBuff)
{
	int n = 0;
	if (NULL == pBuff)
	{
		goto err;
	}

	return (pBuff->bufferSize - getRingBufferSize(pBuff));

err:
	return n;
}

int dumpBuffer(const pRingBuffer pBuff, void* pData)
{
	if (NULL == pBuff || NULL == pData)
	{
		goto err;
	}

	if (BufferEmptyYes == pBuff->empty)
	{
		goto err;
	}

	int count = 0;
	int head = pBuff->head;
	int tail = pBuff->tail;

	// if buffer is full state, pre-read one element
	if (isFull(pBuff))
	{
		memcpy(((unsigned char*)pData + count * pBuff->elementSize),
			(pBuff->pData + tail * pBuff->elementSize),
			pBuff->elementSize);
		count++;
		tail++;
		if (tail >= pBuff->bufferSize)
		{
			tail = 0;
		}
	}

	while (tail != head)
	{
		memcpy(((unsigned char*)pData + count * pBuff->elementSize),
			(pBuff->pData + tail * pBuff->elementSize), 
			pBuff->elementSize);
		count++;
		tail++;
		if (tail >= pBuff->bufferSize)
		{
			tail = 0;
		}
	}

	return count;

err:
	return 0;
}

