#pragma once
#include <mutex>
#include <vector>

/*
	Function definition needs to be declared as:
		bool __fastcall Func(void* m_GenericArg, void* m_Arg)

	Return Value:
		false - Remove callback
		true - Continue running callback
*/
struct CallbackPP_t
{
	std::mutex m_Mutex;
	std::vector<std::pair<void*, void*>> m_Funcs;

	__inline bool TryLock()
	{
		return m_Mutex.try_lock();
	}

	__inline void Unlock()
	{
		m_Mutex.unlock();
	}

	__inline std::vector<std::pair<void*, void*>>::iterator Begin()
	{
		return m_Funcs.begin();
	}

	__inline std::vector<std::pair<void*, void*>>::iterator End()
	{
		return m_Funcs.end();
	}

	__inline std::vector<std::pair<void*, void*>>::iterator Remove(std::vector<std::pair<void*, void*>>::iterator m_It)
	{
		return m_Funcs.erase(m_It);
	}

	// Don't use this, if you don't have mutex locked!
	__inline void RunUnsafe(void* m_GenericArg)
	{
		for (auto m_It = Begin(); m_It != End();)
		{
			std::pair<void*, void*>& m_Callback = *m_It;

			if (reinterpret_cast<bool(__fastcall*)(void*, void*)>(m_Callback.first)(m_GenericArg, m_Callback.second))
				++m_It;
			else
				m_It = Remove(m_It);
		}
	}

	/*
	*	If you need to have callbacks run now, use this!
	*	WARNING: This will do 'spinlock' till mutex is available again.
	*/
	__declspec(noinline) void ForceRun(void* m_GenericArg = nullptr)
	{
		std::lock_guard<std::mutex> m_LockGuard(m_Mutex);
		RunUnsafe(m_GenericArg);
	}

	/* 
	*	This is generic run, but it can fail if there is other thread adding new callback.
	*	Check 'ForceRun', if you need to run callbacks now.
	*/
	__inline void Run(void* m_GenericArg = nullptr)
	{
		if (!TryLock())
			return;

		RunUnsafe(m_GenericArg);

		Unlock();
	}

	void Add(void* m_Function, void* m_Arg = nullptr)
	{
		std::lock_guard<std::mutex> m_LockGuard(m_Mutex);
		m_Funcs.emplace_back(m_Function, m_Arg);
	}

	bool AddOnce(void* m_Function, void* m_Arg = nullptr)
	{
		std::lock_guard<std::mutex> m_LockGuard(m_Mutex);

		for (auto& Pair : m_Funcs)
		{
			// Already exist...
			if (Pair.first == m_Function && Pair.second == m_Arg)
				return false;
		}

		m_Funcs.emplace_back(m_Function, m_Arg);
		return true;
	}
};
