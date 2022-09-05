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
	SafeQueue() {}
	~SafeQueue() {}

	void opStart()
	{
		m_mtx.lock();
	}

	void opEnd()
	{
		m_mtx.unlock();
	}

	CommonBoost::Mutex& getMutex()
	{
		return m_mtx;
	}

	bool empty()
	{
		return m_queue.empty();
	}

	SizeType size()
	{
		return m_queue.size();
	}

	void push(const ValueType& val)
	{
		m_queue.push(val);
	}

	void push(ValueType&& val)
	{
		m_queue.push(val);
	}

	void pop()
	{
		m_queue.pop();
	}

	Reference front()
	{
		return m_queue.front();
	}

	ConstReference front() const
	{
		return m_queue.front();
	}

	Reference back()
	{
		return m_queue.back();
	}

	ConstReference back() const
	{
		return m_queue.back();
	}

private:
	QueueType m_queue;

	CommonBoost::Mutex m_mtx;
};

#endif // !__SAFE_QUEUE_H__
