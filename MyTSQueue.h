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

	bool IsEmpty() const;

private:
	std::queue<std::string> queue_;
	mutable std::mutex mutex_;
	std::condition_variable cond_var_;
};

}