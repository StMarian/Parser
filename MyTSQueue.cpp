#include "MyTSQueue.h"

namespace CFParser
{

MyTSQueue::MyTSQueue(void) : queue_(), mutex_(), cond_var_()
{
}

void MyTSQueue::Push(std::string rs)
{
	std::lock_guard<std::mutex> locker(mutex_);
	queue_.emplace(rs);
	cond_var_.notify_one();
}

std::string MyTSQueue::Pop()
{
	std::unique_lock<std::mutex> locker(mutex_);

	while (queue_.empty())
		cond_var_.wait(locker);

	std::string rs = queue_.front();
	queue_.pop();
	return rs;
}

bool MyTSQueue::IsEmpty() const
{
	std::lock_guard<std::mutex> locker(mutex_);
	return queue_.empty();
}

}