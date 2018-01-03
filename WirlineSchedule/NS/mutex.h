#ifndef _MUTEX_H
#define _MUTEX_H

#ifdef _WIN32
#define LOCKERTYPE				HANDLE
#define INITLOCKER()			CreateMutex(NULL, FALSE, NULL)
#define TRYLOCK(locker)			WaitForSingleObject(locker, INFINITE)
#define RELEASELOCK(locker)		ReleaseMutex(locker)
#define DELETELOCKER(locker)	CloseHandle(locker);
#else // _WIN32
#define LOCKERTYPE				pthread_mutex_t
#define INITLOCKER()			PTHREAD_MUTEX_INITIALIZER
#define TRYLOCK(locker)			pthread_mutex_lock(&locker)
#define RELEASELOCK(locker)		pthread_mutex_unlock(&locker)
#define DELETELOCKER(locker)	pthread_mutex_destroy(&locker);
#endif 

#endif // !_MUTEX_H
