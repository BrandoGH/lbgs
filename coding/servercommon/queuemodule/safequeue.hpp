#ifndef __SAFE_QUEUE_H__
#define __SAFE_QUEUE_H__

#include <queue>
#include <boostmodule/basedef.h>

/*
*	对std::queue简单封装
*	具有线程安全性质的队列
* 
* 1.在opStart()和opEnd()之间进行队列操作，这是这种方法要注意，是否会产生死锁
* 2.外部可调用CommonBoost::UniqueLock(getMutex())方法，推荐使用
* 3.当然，也可以在操作开始前设置setAutoLock自动锁，后续的操作不用手动去向第2点那样加锁，这个看需求调用，也推荐
*
*/
template <class InputDataType, class _Container = std::deque<InputDataType>>
class SafeQueue
{
public:
	typedef std::queue<InputDataType, _Container> QueueType;
	// typedef typename标明是一个类型而不是变量
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
		操作开始自动加锁，应该在所有操作前去调用，比如构造
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
