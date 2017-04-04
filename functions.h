#pragma once
#include <atomic>
#include <regex>
#include <iostream>

#include "filesystem\include\boost\filesystem.hpp"

#include "MyTSQueue.h"

using uli = unsigned long int;
using clk = std::chrono::steady_clock;

namespace CFParser
{

struct Data
{
	Data() : files(), rx(R"(.*\.(c|h|cpp|hpp)$)"), searching(true), parsing_time(0), files_cnt(0), all_lines_cnt(0), blank_lines_cnt(0), comment_lines_cnt(0)
	{}

	MyTSQueue files;
	std::regex rx;
	bool searching;
	long long parsing_time;

	std::atomic<uli> files_cnt;
	std::atomic<uli> all_lines_cnt;
	std::atomic<uli> blank_lines_cnt;
	std::atomic<uli> comment_lines_cnt;
};

void Search(boost::filesystem::path root_folder, Data& data);
void Parse(Data& data);
void SaveResults(const Data& data, std::ostream& s = std::cout);

}

