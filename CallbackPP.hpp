#pragma once
#include <mutex>

typedef bool(__fastcall* m_tCallbackPP)(void*, void*);
/*
	Function definition needs to be declared as:
		bool __fastcall Func(void* m_Class, void* m_Data)

	Return Value:
		false - Remove callback
		true - Continue running callback
*/

struct CallbackPP_t
{
	std::mutex m_Mutex;

	struct Node_t
	{
		Node_t* m_Next = nullptr;
		m_tCallbackPP m_Callback = nullptr;
		void* m_Class = nullptr;
		void* m_Data = nullptr;

		Node_t(m_tCallbackPP p_Callback, void* p_Class, void* p_Data = nullptr)
		{
			m_Callback	= p_Callback;
			m_Class		= p_Class;
			m_Data		= p_Data;
		}
	};
	Node_t* m_Nodes			= nullptr;
	Node_t* m_QueueNodes	= nullptr;

	__inline void MoveQueueNodes()
	{
		Node_t* m_LastQueueNode = m_QueueNodes;
		while (m_LastQueueNode->m_Next)
			m_LastQueueNode = m_LastQueueNode->m_Next;

		m_LastQueueNode->m_Next = m_Nodes;
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

		Node_t* m_PrevNode	= nullptr;
		Node_t* m_CurNode	= m_Nodes;
		while (m_CurNode)
		{
			if (m_CurNode->m_Callback(m_CurNode->m_Class, m_CurNode->m_Data))
			{
				m_PrevNode	= m_CurNode;
				m_CurNode	= m_CurNode->m_Next;
				continue;
			}

			// Remove node
			Node_t* m_NextNode = m_CurNode->m_Next;
			delete m_CurNode;

			if (m_PrevNode)
				m_PrevNode->m_Next = m_NextNode;
			else
				m_Nodes = m_NextNode;

			m_CurNode = m_NextNode;
		}
	}

	void Add(void* p_Function, void* p_Class, void* p_Data = nullptr)
	{
		m_Mutex.lock();
		{
			Node_t* m_NewNode = new Node_t(reinterpret_cast<m_tCallbackPP>(p_Function), p_Class, p_Data);
			m_NewNode->m_Next = m_QueueNodes;
			m_QueueNodes = m_NewNode;
		}
		m_Mutex.unlock();
	}
};
