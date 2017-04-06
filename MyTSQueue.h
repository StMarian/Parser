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

	void Push(std::string rs);
	std::string Pop();

	bool Empty() const;

private:
	std::queue<std::string> m_queue;
	mutable std::mutex m_mutex;
	std::condition_variable m_cond_var;
};

}