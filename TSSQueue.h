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
	TSSQueue() : m_qu(), m_mu(), m_cv()
	{}

	void Push(std::string rs)
	{
		std::lock_guard<std::mutex> locker(m_mu);
		m_qu.emplace(rs);
		m_cv.notify_one();
	}

	std::string Pop()
	{
		std::unique_lock<std::mutex> locker(m_mu);

		while (m_qu.empty())
			m_cv.wait(locker);

		std::string rs = m_qu.front();
		m_qu.pop();
		return rs;
	}

	bool Empty() const { return m_qu.empty(); }

private:
	std::queue<std::string> m_qu;
	mutable std::mutex m_mu;
	std::condition_variable m_cv;
};

}