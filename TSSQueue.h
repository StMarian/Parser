#pragma once
#include <string>
#include <mutex>
#include <queue>
#include <condition_variable>

namespace CFParser
{

class TSSQueue final
{
public:
	TSSQueue() : m_queue(), m_mutex(), m_cond_var()
	{}

	void Push(std::string rs)
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		m_queue.emplace(rs);
		m_cond_var.notify_one();
	}

	std::string Pop()
	{
		std::unique_lock<std::mutex> locker(m_mutex);

		while (m_queue.empty())
			m_cond_var.wait(locker);

		std::string rs = m_queue.front();
		m_queue.pop();
		return rs;
	}

	bool Empty() const 
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		return m_queue.empty();
	}

private:
	std::queue<std::string> m_queue;
	mutable std::mutex m_mutex;
	std::condition_variable m_cond_var;
};

}