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
* 3.��Ȼ��Ҳ�����ڲ�����ʼǰ����setAutoLock�Զ����������Ĳ��������ֶ�ȥ���2�����������������������ã�Ҳ�Ƽ�
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
		������ʼ�Զ�������Ӧ�������в���ǰȥ���ã����繹��
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
