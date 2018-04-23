#pragma once

template<typename T>   //这里的T指的是 ： CTest类
class CMemoryPool
{
private:
	CMemoryPool<T>* m_pFreeList;

public:
	enum { EXPANSION_SIZE = 32 };

	CMemoryPool(unsigned int nItemCount = EXPANSION_SIZE)
	{
		ExpandFreeList(nItemCount);
	}

	~CMemoryPool()
	{
		//free all memory in the list
		CMemoryPool<T>* pNext = NULL;
		for (pNext = m_pFreeList; pNext != NULL; pNext = m_pFreeList)
		{
			m_pFreeList = m_pFreeList->m_pFreeList;
			delete[](char*)pNext;
		}
	}

	void* Alloc(unsigned int /*size*/)
	{
		if (m_pFreeList == NULL)
		{
			ExpandFreeList();
		}

		//get free memory from head
		CMemoryPool<T>* pHead = m_pFreeList;
		m_pFreeList = m_pFreeList->m_pFreeList;
		return pHead;
	}

	void Free(void* p)
	{
		//push the free memory back to list
		CMemoryPool<T>* pHead = static_cast<CMemoryPool<T>*>(p);
		pHead->m_pFreeList = m_pFreeList;
		m_pFreeList = pHead;
	}

protected:
	//allocate memory and push to the list
	void ExpandFreeList(unsigned nItemCount = EXPANSION_SIZE)
	{
		unsigned int nSize = sizeof(T) > sizeof(CMemoryPool<T>*) ? sizeof(T) : sizeof(CMemoryPool<T>*);		//取大的

		//申请一段空间
		CMemoryPool<T>* pLastItem = static_cast<CMemoryPool<T>*>(static_cast<void*>(new char[nSize]));
		m_pFreeList = pLastItem;

		for (int i = 0; i<nItemCount - 1; ++i)
		{
			pLastItem->m_pFreeList = static_cast<CMemoryPool<T>*>(static_cast<void*>(new char[nSize]));
			pLastItem = pLastItem->m_pFreeList;
		}

		pLastItem->m_pFreeList = NULL;
	}


};

