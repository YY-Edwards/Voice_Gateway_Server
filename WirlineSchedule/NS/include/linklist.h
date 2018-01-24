#ifndef _LINK_LIST_H
#define _LINK_LIST_H

#include "global.h"

#ifdef __cplusplus
extern "C" {
#endif

	/*
	* Link list match function prototype
	* @return 0: not match, 1:matched
	*/
	typedef bool(*LinkMatchFunc)(const void* pValue, const void* pCondition);

	typedef struct _Link_list_item
	{
		struct _Link_list_item* pNext;
		void *data;
	}LinkItem, *pLinkList, *pLinkItem;

	/*
	* Create a link list
	*/
	pLinkList createLinkList();
	
	/*
	* Initialize link item
	*/
	void initLinkItem(pLinkItem pItem);

	/*
	* Append a item into link, not take the ownership of item
	*/
	void appendData(pLinkList* pList, void* pData);

	/*
	* Append a link list item
	*/
	void appendItem(pLinkList* pList, pLinkItem pItem);

	/*
	* Find item by match function
	*/
	pLinkItem findItem(const pLinkList pList, const void* pCondition, LinkMatchFunc matcher);

	/*
	* Free entire list
	*/
	void freeList(pLinkList pList);

	/*
	* Count list size
	*/
	int listSize(const pLinkList pList);

	/*
	* Remove item from link list
	*
	* @param pList pLinkList, link list
	* @param pData const void*, link item content pointer for match
	* @return link list item pointer
	*/
	pLinkItem removeItem(pLinkList* pList, const void* pData);

	/*
	* Pop front element
	*/
	pLinkItem popFront(pLinkList* pList);

	/*
	* Test if the list is empty
	*/
	bool isEmptyList(pLinkList* pList);

#ifdef __cplusplus
}
#endif
#endif
