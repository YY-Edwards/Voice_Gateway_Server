#include <string.h>
#include "linklist.h"

#ifndef NULL
#define NULL	((void*)0)
#endif // !NULL

pLinkList createLinkList()
{
	pLinkList pItem = (pLinkList)malloc(sizeof(LinkItem));
	if (NULL == pItem)
	{
		return (void*)0;
	}

	initLinkItem(pItem);
	return pItem;
}

void initLinkItem(pLinkItem pItem)
{
	if (NULL == pItem)
	{
		return;
	}

	memset(pItem, 0, sizeof(LinkItem));
}

void appendData(pLinkList* pList, void* pData)
{
	if (NULL == pData)
	{
		return;
	}

	pLinkItem p = (pLinkItem)malloc(sizeof(LinkItem));
	if (NULL == p)
	{
		return;
	}
	initLinkItem(p);
	p->data = pData;

	appendItem(pList, p);
}

void appendItem(pLinkList* pList, pLinkItem pItem)
{
	// find last element
	if (NULL == *pList)
	{
		*pList = pItem;
	}
	else
	{
		pLinkList pTemp = *pList;
		while (NULL != pTemp->pNext)
		{
			pTemp = pTemp->pNext;
		}
		pTemp->pNext = pItem;
	}
}

pLinkItem findItem(const pLinkList pList, const void* pCondition, LinkMatchFunc matcher)
{
	pLinkItem pTemp = pList;
	if (NULL == pList)
	{
		return NULL;
	}

	do 
	{
		if (matcher(pTemp->data, pCondition))
		{
			return pTemp;
		}
		pTemp = pTemp->pNext;
	} while (NULL != pTemp);

	return NULL;
}

int listSize(const pLinkList pList)
{
	if (NULL == pList)
	{
		return 0;
	}
	int s = 0;
	pLinkList pTemp = pList;

	do 
	{
		s++;
		pTemp = pTemp->pNext;
	} while (NULL != pTemp);

	return s;
}

pLinkItem removeItem(pLinkList* pList, const void* pData)
{
	if (NULL == pData || NULL == pList)
	{
		return NULL;
	}

	pLinkList p = *pList;
	pLinkList pPrev = NULL;
	do 
	{
		if (NULL == p)
		{
			return NULL;
		}
		if (p->data == pData)
		{
			if (NULL == pPrev)
			{
				*pList = p->pNext;
			}
			else
			{
				pPrev->pNext = p->pNext;
			}

			return p;
		}
		pPrev = p;
		p = p->pNext;
	} while (NULL != p);

	return NULL;
}

pLinkItem popFront(pLinkList* pList)
{
	if (NULL == *pList)
	{
		return NULL;
	}

	pLinkItem p = *pList;
	*pList = (*pList)->pNext;
	p->pNext = NULL;

	return p;
}

bool isEmptyList(pLinkList* pList)
{
	if (NULL == pList)
	{
		return true;
	}

	if (NULL == *pList)
	{
		return true;
	}

	return false;
}

void freeList(pLinkList pList)
{
	while (pList)
	{
		pLinkList header = pList->pNext;
		pList->pNext = NULL;
		free(pList);
		pList = header;
	}
}
