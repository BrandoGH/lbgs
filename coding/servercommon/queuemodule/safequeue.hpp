#ifndef __SAFE_QUEUE_H__
#define __SAFE_QUEUE_H__

#include <queue>
#include <boostmodule/basedef.h>

/*
*	��std::queue�򵥷�װ
*	�����̰߳�ȫ���ʵĶ���
* 
* 1.��opStart()��opEnd()֮����ж��в������������ַ���Ҫע�⣬�Ƿ���������
* 2.�ⲿ�ɵ���CommonBoost::UniqueLock(getMutex())�������Ƽ�ʹ��
*
*/
template <class InputDataType, class _Container = std::deque<InputDataType>>
class SafeQueue
{
public:
	typedef std::queue<InputDataType, _Container> QueueType;
	// typedef typename������һ�����Ͷ����Ǳ���
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
