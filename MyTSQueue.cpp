#include "MyTSQueue.h"

namespace CFParser
{

MyTSQueue::MyTSQueue(void) : m_queue(), m_mutex()
{
}

std::string MyTSQueue::Pop()
{
	std::lock_guard<std::mutex> locker(m_mutex);
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