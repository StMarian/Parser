#include "Data.h"

#include <thread>	
#include <chrono>
#include <vector>

using namespace CFParser;

int main(int argc, char** argv)
{
	// argc[0] - system info
	boost::filesystem::path root_folder = (argc > 1) ? argv[1] : R"(D:\~SS\VS_SS_workspace\CFP_00\CFP_00\FindMe)";
	std::string info_file_name = (argc > 2) ? argv[2] : "info.txt";

	std::cout << "Root folder: " << root_folder << std::endl;

	unsigned int optimum_thread_count = std::thread::hardware_concurrency();
	std::cout << "Optimum thread count: " << optimum_thread_count << std::endl;

	Data data;
	auto search_fn = std::bind(&Data::Search, std::ref(data), root_folder);
	auto parse_fn = std::bind(&Data::Parse, std::ref(data));

	std::vector<std::thread> parsers;
	clk::time_point begin = clk::now();

	std::thread searcher(search_fn);

	for (unsigned int i = 0; i < optimum_thread_count - 1; i++)
		parsers.emplace_back(std::thread(parse_fn));

	searcher.join();

	for (unsigned int i = 0; i < optimum_thread_count - 1; i++)
		parsers[i].join();

	clk::time_point finish = clk::now();
	auto total_time = std::chrono::duration_cast<std::chrono::microseconds>(finish - begin).count() / CLOCKS_PER_SEC;
	data.set_m_total_time(total_time);

	// Information console output 
	data.SaveResults();

	// Information saving to file
	std::ofstream save_info;
	save_info.open(info_file_name);
	if (!save_info.is_open())
	{
		std::cout << "Error opening log file" << std::endl;
	}
	else
	{
		data.SaveResults(save_info);
		save_info.close();
	}

	system("pause");
	return 0;
}