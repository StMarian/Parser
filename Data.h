#pragma once
#include "MyTSQueue.h"

#include "filesystem\include\boost\filesystem.hpp"

#include <atomic>
#include <regex>
#include <iostream>

using uli = unsigned long int;
using clk = std::chrono::steady_clock;

namespace CFParser
{

class Data
{
public:
	Data() : files_(), iam_searching_(true),  rx_(R"(.*\.(c|h|cpp|hpp)$)"), total_time_(0)
		   , files_cnt_(0), all_lines_cnt_(0), blank_lines_cnt_(0), comment_lines_cnt_(0)
	{}

	void Search(const boost::filesystem::path& root_folder);
	void Parse();
	void SaveResults(std::ostream& s = std::cout);

	void set_m_total_time(unsigned long time) { total_time_ = time; };

private:
	MyTSQueue files_;
	bool iam_searching_;
	std::regex rx_;
	unsigned long total_time_;

	std::atomic<uli> files_cnt_;
	std::atomic<uli> all_lines_cnt_;
	std::atomic<uli> blank_lines_cnt_;
	std::atomic<uli> comment_lines_cnt_;
};

}

