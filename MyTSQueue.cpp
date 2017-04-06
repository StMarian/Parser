#include "MyTSQueue.h"

namespace CFParser
{

MyTSQueue::MyTSQueue(void) : m_queue(), m_mutex(), m_cond_var()
{
}

void MyTSQueue::Push(std::string rs)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	m_queue.emplace(rs);
	m_cond_var.notify_one();
}

std::string MyTSQueue::Pop()
{
	std::unique_lock<std::mutex> locker(m_mutex);

	while (m_queue.empty())
		m_cond_var.wait(locker);

	std::string rs = m_queue.front();
	m_queue.pop();
	return rs;
}

bool MyTSQueue::Empty() const
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_queue.empty();
}

}