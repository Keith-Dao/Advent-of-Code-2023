#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

class Solver
{
	std::filesystem::path filepath;

	long calculate_distance(long total_time, long time_held)
	{
		return time_held * (total_time - time_held);
	}

	std::pair<long, long> find_minimum_and_maximum_valid_hold_times(
			long race_time, long minimum_distance)
	{
		long left = 0, right = race_time + 1, time;

		// Find minimum valid hold time
		while (left < right)
		{
			time = left + (right - left) / 2;

			if (this->calculate_distance(race_time, time) > minimum_distance)
			{
				right = time;
			}
			else
			{
				left = time + 1;
			}
		}
		int minimum_hold_time = left;

		// Find maximum valid hold time
		right = race_time;
		while (left < right)
		{
			time = right - (right - left) / 2;

			if (this->calculate_distance(race_time, time) > minimum_distance)
			{
				left = time;
			}
			else
			{
				right = time - 1;
			}
		}
		int maximum_hold_time = left;

		return std::make_pair(minimum_hold_time, maximum_hold_time);
	}

	long part_1()
	{
		auto parse_line = [](std::ifstream &file) -> std::vector<long>
		{
			char c;

			// Skip labels
			while ((c = file.get()) != ':')
			{
			}
			while ((c = file.get()) == ' ')
			{
			}

			std::vector<long> result;
			long num = -1;
			for (; c != '\n' && c != EOF; c = file.get())
			{
				if (c == ' ')
				{
					if (num != -1)
						result.push_back(num);
					num = -1;
					continue;
				}

				if (num == -1)
					num = 0;
				num = num * 10 + c - '0';
			}
			result.push_back(num);

			return result;
		};

		std::ifstream file;
		file.open(this->filepath);
		std::vector<long> race_times = parse_line(file), distances = parse_line(file);
		file.close();

		long result = 1;
		for (int i = 0; i < race_times.size(); ++i)
		{
			auto [min, max] = this->find_minimum_and_maximum_valid_hold_times(race_times[i], distances[i]);
			result *= max - min + 1;
		}
		return result;
	}

	int part_2()
	{
		auto parse_line = [](std::ifstream &file) -> long
		{
			char c;

			// Skip labels
			while ((c = file.get()) != ':')
			{
			}
			while ((c = file.get()) == ' ')
			{
			}

			long result = 0;
			for (; c != '\n' && c != EOF; c = file.get())
			{
				if (c == ' ')
					continue;

				result = result * 10 + c - '0';
			}

			return result;
		};

		std::ifstream file;
		file.open(this->filepath);
		long race_time = parse_line(file), distance = parse_line(file);
		file.close();

		auto [min, max] = this->find_minimum_and_maximum_valid_hold_times(race_time, distance);
		return max - min + 1;
	}

public:
	Solver(std::filesystem::path filepath = "input.txt") : filepath(filepath) {}

	void solve()
	{
		std::cout << "Day 6" << std::endl;
		std::cout << "Solving: " << this->filepath << std::endl;
		std::cout << "Part 1:" << std::endl;
		std::cout << this->part_1() << std::endl;
		std::cout << "---" << std::endl;
		std::cout << "Part 2:" << std::endl;
		std::cout << this->part_2() << std::endl;
	}
};

int main(int argc, char *argv[])
{
	std::unique_ptr<Solver> solver;
	if (argc == 1)
	{
		solver = std::make_unique<Solver>();
	}
	else if (argc == 2)
	{
		solver = std::make_unique<Solver>(argv[1]);
	}
	else if (argc == 3 && (argv[1] == "-p" || argv[1] == "--path"))
	{
		solver = std::make_unique<Solver>(argv[2]);
	}
	else
	{
		return 1;
	}

	solver->solve();
}