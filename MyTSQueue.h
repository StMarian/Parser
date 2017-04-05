#pragma once
#include <string>
#include <queue>
#include <mutex>

namespace CFParser
{

class MyTSQueue
{
public:
	MyTSQueue(void);

	// Only one thread would call this method, so it needn't to be thread-safe
	void Push(std::string rs) { m_queue.emplace(rs); };
	std::string Pop();

	bool Empty() const;

private:
	std::queue<std::string> m_queue;
	mutable std::mutex m_mutex;
};

}