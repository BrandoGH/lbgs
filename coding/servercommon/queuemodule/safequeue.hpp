#ifndef __SAFE_QUEUE_H__
#define __SAFE_QUEUE_H__

#include <queue>
#include <boostmodule/basedef.h>

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

	bool empty() const
	{
		CommonBoost::UniqueLock lock(m_mtx);
		return m_queue.empty();
	}

	SizeType size() const
	{
		return m_queue.size();
	}

	void push(const ValueType& val)
	{
		CommonBoost::UniqueLock lock(m_mtx);
		m_queue.push(val);
	}

	void push(ValueType&& val)
	{
		CommonBoost::UniqueLock lock(m_mtx);
		m_queue.push(val);
	}

	void pop()
	{
		CommonBoost::UniqueLock lock(m_mtx);
		m_queue.pop();
	}

	Reference front()
	{
		CommonBoost::UniqueLock lock(m_mtx);
		return m_queue.front();
	}

	ConstReference front() const
	{
		CommonBoost::UniqueLock lock(m_mtx);
		return m_queue.front();
	}

	Reference back()
	{
		CommonBoost::UniqueLock lock(m_mtx);
		return m_queue.back();
	}

	ConstReference back() const
	{
		CommonBoost::UniqueLock lock(m_mtx);
		return m_queue.back();
	}

private:
	QueueType m_queue;

	CommonBoost::Mutex m_mtx;
};

#endif // !__SAFE_QUEUE_H__
