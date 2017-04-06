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

class Data
{
public:
	Data() : m_files(), m_iam_searching(true),  m_rx(R"(.*\.(c|h|cpp|hpp)$)"), m_total_time(0), m_files_cnt(0), m_all_lines_cnt(0), m_blank_lines_cnt(0), m_comment_lines_cnt(0)
	{}

	void Search(boost::filesystem::path root_folder);
	void Parse();
	void SaveResults(std::ostream& s = std::cout);

	void set_m_total_time(unsigned long time) { m_total_time = time; };

private:
	MyTSQueue m_files;
	bool m_iam_searching;
	std::regex m_rx;
	unsigned long m_total_time;

	std::atomic<uli> m_files_cnt;
	std::atomic<uli> m_all_lines_cnt;
	std::atomic<uli> m_blank_lines_cnt;
	std::atomic<uli> m_comment_lines_cnt;
};

}

