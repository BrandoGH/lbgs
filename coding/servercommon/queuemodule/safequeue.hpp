#ifndef __SAFE_QUEUE_H__
#define __SAFE_QUEUE_H__

#include <queue>
#include <boostmodule/basedef.h>

/*
*	Simple encapsulation of std::queue
*	Queues with thread-safe properties
*
* 1. Queue operation between opStart() and opEnd(), this is the method to pay attention to, whether it will cause deadlock
* 2. CommonBoost::UniqueLock(getMutex()) method can be called externally, it is recommended to use
* 3. Of course, you can also set the setAutoLock automatic lock before the operation starts, 
		and the subsequent operations do not need to be manually locked like the second point. This depends on the demand call, and it is also recommended
*
*/
template <class InputDataType, class _Container = std::deque<InputDataType>>
class SafeQueue
{
public:
	typedef std::queue<InputDataType, _Container> QueueType;
	// typedef typename indicates a type rather than a variable
	typedef typename _Container::value_type			ValueType;
	typedef typename _Container::size_type			SizeType;
	typedef typename _Container::reference			Reference;
	typedef typename _Container::const_reference	ConstReference;
public:
	SafeQueue() 
		: m_bAutoLock(false)
	{}
	~SafeQueue() {}

	void opStart()
	{
		m_mtx.lock();
	}

	void opEnd()
	{
		m_mtx.unlock();
	}

	/*
		The operation starts to be automatically locked and should be called before all operations, such as constructing
	*/
	void setAutoLock(bool bAutoLock)
	{
		m_bAutoLock = bAutoLock;
	}

	CommonBoost::Mutex& getMutex()
	{
		return m_mtx;
	}

	bool empty()
	{
		if (m_bAutoLock)
		{
			CommonBoost::UniqueLock lock(m_mtx);
		}
		return m_queue.empty();
	}

	SizeType size()
	{
		if (m_bAutoLock)
		{
			CommonBoost::UniqueLock lock(m_mtx);
		}
		return m_queue.size();
	}

	void push(const ValueType& val)
	{
		if (m_bAutoLock)
		{
			CommonBoost::UniqueLock lock(m_mtx);
		}
		m_queue.push(val);
	}

	void push(ValueType&& val)
	{
		if (m_bAutoLock)
		{
			CommonBoost::UniqueLock lock(m_mtx);
		}
		m_queue.push(val);
	}

	void pop()
	{
		if (m_bAutoLock)
		{
			CommonBoost::UniqueLock lock(m_mtx);
		}
		m_queue.pop();
	}

	Reference front()
	{
		if (m_bAutoLock)
		{
			CommonBoost::UniqueLock lock(m_mtx);
		}
		return m_queue.front();
	}

	ConstReference front() const
	{
		if (m_bAutoLock)
		{
			CommonBoost::UniqueLock lock(m_mtx);
		}
		return m_queue.front();
	}

	Reference back()
	{
		if (m_bAutoLock)
		{
			CommonBoost::UniqueLock lock(m_mtx);
		}
		return m_queue.back();
	}

	ConstReference back() const
	{
		if (m_bAutoLock)
		{
			CommonBoost::UniqueLock lock(m_mtx);
		}
		return m_queue.back();
	}

private:
	QueueType m_queue;
	bool m_bAutoLock;
	CommonBoost::Mutex m_mtx;
};

#endif // !__SAFE_QUEUE_H__
