#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <cassert>
#include <unordered_set>
#include <functional>
#include <limits>
#include <stack>

constexpr auto max_size = std::numeric_limits<std::streamsize>::max();

struct PairHash
{
public:
	template <typename T, typename U>
	std::size_t operator()(const std::pair<T, U> &x) const
	{
		return std::hash<T>()(x.first) ^ std::hash<U>()(x.second);
	}
};

class Solver
{
	std::filesystem::path filepath;

	template <typename T>
	void template_solve(std::ifstream &file, T &prev_nums, std::function<void(T &, T &, long, long, long)> solve)
	{
		file.ignore(max_size, '\n'); // Blank line separating seed and first map
		file.ignore(max_size, '\n'); // Map header line

		while (!file.eof()) // Read one map
		{
			T next_nums;
			while (!file.eof() && file.peek() != '\n') // Read one line
			{
				// Parse line in the form: destination source range
				char c = file.get();
				long destination = 0, source = 0, range = 0;

				// Destination
				while (c != ' ')
				{
					destination = destination * 10 + c - '0';
					c = file.get();
				}

				// Source
				c = file.get();
				while (c != ' ')
				{
					source = source * 10 + c - '0';
					c = file.get();
				}

				// Range
				c = file.get();
				while (c != '\n' && c != EOF)
				{
					range = range * 10 + c - '0';
					c = file.get();
				}

				solve(prev_nums, next_nums, destination, source, range);
			}

			file.ignore(max_size, '\n'); // Blank line separating maps
			file.ignore(max_size, '\n'); // Next map's header line or EOF

			prev_nums.merge(next_nums);
		}
	}

	long part_1()
	{
		std::ifstream file;
		file.open(this->filepath);

		// Get seeds
		std::unordered_set<long> prev_nums;
		char c = file.get();
		while (c != ':')
			c = file.get();
		c = file.get(); // Space

		long num = 0;
		while ((c = file.get()) != '\n')
		{
			if (c != ' ')
			{
				num = num * 10 + c - '0';
				continue;
			}
			prev_nums.insert(num);
			num = 0;
		}
		prev_nums.insert(num);

		// Solve to find locations
		auto solve = [](std::unordered_set<long> &prev_nums, std::unordered_set<long> &next_nums, long destination, long source, long range)
		{
			std::vector<long> nums(prev_nums.begin(), prev_nums.end());
			for (long num : nums)
			{
				if (!(source <= num && num < source + range))
				{
					continue;
				}

				next_nums.insert(destination + num - source);
				prev_nums.erase(num);
			}
		};
		this->template_solve<std::unordered_set<long>>(file, prev_nums, solve);

		file.close();
		return *std::min_element(prev_nums.begin(), prev_nums.end());
	}

	int part_2()
	{
		std::ifstream file;
		file.open(this->filepath);

		// Get seeds
		typedef std::pair<long, long> pair;
		typedef std::unordered_set<pair, PairHash> set;
		set prev_nums;
		char c = file.get();
		while (c != ':')
			c = file.get();
		c = file.get(); // Space

		long num = 0, first = -1;
		while ((c = file.get()) != '\n')
		{
			if (c != ' ')
			{
				num = num * 10 + c - '0';
				continue;
			}

			if (first != -1)
				prev_nums.emplace(first, first + num - 1);

			first = first == -1 ? num : -1;
			num = 0;
		}
		prev_nums.emplace(first, first + num - 1);

		// Solve to find locations
		auto solve = [](set &prev_ranges, set &next_ranges, long destination, long source, long range_length)
		{
			std::stack<pair> stack;
			for (const pair &range : prev_ranges)
			{
				stack.push(range);
			}

			while (!stack.empty())
			{
				pair range = stack.top(), next_range;
				stack.pop();
				if (range.second < source || source + range_length - 1 < range.first)
					continue;

				prev_ranges.erase(range);
				next_ranges.emplace(
						destination + std::max(source, range.first) - source,
						destination + std::min(source + range_length - 1, range.second) - source);

				if (range.first < source)
				{
					next_range = pair(range.first, source - 1);
					stack.push(next_range);
					prev_ranges.insert(next_range);
				}

				if (source + range_length - 1 < range.second)
				{
					next_range = pair(source + range_length, range.second);
					stack.push(next_range);
					prev_ranges.insert(next_range);
				}
			}
		};
		this->template_solve<set>(file, prev_nums, solve);

		file.close();
		return std::min_element(prev_nums.begin(), prev_nums.end())->first;
	}

public:
	Solver(std::filesystem::path filepath = "input.txt") : filepath(filepath) {}

	void solve()
	{
		std::cout << "Day 5" << std::endl;
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