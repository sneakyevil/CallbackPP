#pragma once
#include <mutex>

typedef bool(__fastcall* Fn_CallbackPP)(void*, void*);
/*
	Function definition needs to be declared as:
		- bool __fastcall Func(void* pClass, void* pData)
		or
		- bool __fastcall Func(void* pClass)

	Return Value:
		false - Remove callback
		true - Continue running callback
*/

struct CallbackPP_t
{
	std::mutex m_Mutex;

	struct Node_t
	{
		Node_t* m_Next;
		Fn_CallbackPP m_Callback;
		void* m_Class;
		void* m_Data;

		Node_t(Fn_CallbackPP p_Callback, void* p_Class, void* p_Data = nullptr) 
			: m_Callback(p_Callback), m_Class(p_Class), m_Data(p_Data)
		{ }
	};
	Node_t* m_Nodes			= nullptr;
	Node_t* m_QueueNodes	= nullptr;

	__inline void MoveQueueNodes()
	{
		Node_t* pLastQueueNode = m_QueueNodes;
		while (pLastQueueNode->m_Next) {
			pLastQueueNode = pLastQueueNode->m_Next;
		}

		pLastQueueNode->m_Next = m_Nodes;
		m_Nodes = m_QueueNodes;

		m_QueueNodes = nullptr;
	}

	// WARNING: Can cause spin-lock!
	void PushQueueNodes()
	{
		m_Mutex.lock();

		MoveQueueNodes();

		m_Mutex.unlock();
	}

	void Run()
	{
		if (m_QueueNodes && m_Mutex.try_lock())
		{
			MoveQueueNodes();
			m_Mutex.unlock();
		}

		Node_t* pPrevNode = nullptr;
		Node_t* pNode = m_Nodes;
		while (pNode)
		{
			if (pNode->m_Callback(pNode->m_Class, pNode->m_Data))
			{
				pPrevNode = pNode;
				pNode = pNode->m_Next;
				continue;
			}

			// Remove node
			Node_t* pNextNode = pNode->m_Next;
			delete pNode;

			if (pPrevNode) {
				pPrevNode->m_Next = pNextNode;
			}
			else {
				m_Nodes = pNextNode;
			}

			pNode = pNextNode;
		}
	}

	void Add(void* p_Callback, void* p_Class, void* p_Data = nullptr)
	{
		m_Mutex.lock();
		
		Node_t* pNode = new Node_t(reinterpret_cast<Fn_CallbackPP>(p_Callback), p_Class, p_Data);
		{
			pNode->m_Next = m_QueueNodes;
			m_QueueNodes = pNode;
		}

		m_Mutex.unlock();
	}
};
