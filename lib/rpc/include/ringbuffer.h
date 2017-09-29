#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#ifdef __cplusplus
extern "C" {
#endif

#define			BufferEmptyYes			1
#define			BufferEmptyNo			0

#ifndef NULL
#define			NULL				((void*)0)
#endif // !NULL

	/*
	* RingBuffer Type
	*/
	typedef struct _RingBuffer{
		int head;					// header position
		int tail;					// tail position
		int empty;					// buffer status, full or empty or dirty
		int bufferSize;             // buffer size
		int elementSize;            // element size
		unsigned char* pData;       // point to buffer
	} tRingBuffer;

	typedef tRingBuffer* pRingBuffer;

	/*
	* Create ring buffer type
	*
	* @param bufSize int, buffer size
	* @param elSize int, element size
	* @return tRingBuffer type, failed return 0
	*/
	pRingBuffer createRingBuffer(int bufSize, int elSize);

	/*
	* Free ring buffer
	*/
	void freeRingBuffer(pRingBuffer pBuff);

	/*
	* Test if buffer is empty 
	*/
	bool isEmpty(const pRingBuffer pBuff);

	/*
	* Test if buffer is full
	*/
	bool isFull(const pRingBuffer pBuff);

	/*
	* Get valid data length in buffer 
	*/
	int getRingBufferSize(const pRingBuffer pBuff);

	/*
	* Push a element into buffer
	*
	* @param pBuff pRingBuffer, pointer to ring buffer object
	* @param pEl void*, pointer to element
	* @return -1: error, other position in buffer
	*/
	int push(pRingBuffer pBuff, const void* pEl);

	/*
	* Pop front element 
	*
	* @param pBuff pRingBuffer, pointer to ring buffer object
	* @param pEl void*, out parameter, for get pop element from buffer
	* @return int, 0:no element, other element stored in pEl
	*/
	int pop(pRingBuffer pBuff, void* pEl);

	/*
	* Get capacity of ring buffer
	*
	* @param pBuff pRingBuffer, pointer to ring buffer object
	* @return ring buffer length
	*/
	int capacity(const pRingBuffer pBuff);

	/*
	* Get free size of ring buffer
	*
	* @param pBuff pRingBuffer, pointer to ring buffer object
	* @return free size of ring buffer
	*/
	int getFreeSize(const pRingBuffer pBuff);

	/*
	* Dump buffer data
	*
	* @param pBuff pRingBuffer, pointer to ring buffer object
	* @param pData void*, pointer to store buffer data
	* @return size of dumped data
	*/
	int dumpBuffer(const pRingBuffer pBuff, void* pData);

#ifdef __cplusplus
}
#endif

#endif // !RING_BUFFER_H
