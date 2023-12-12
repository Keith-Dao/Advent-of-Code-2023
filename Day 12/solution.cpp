#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <cassert>

typedef std::pair<int, int> coordinate;

class Solver
{
	std::filesystem::path filepath;

	long count_arrangements(const std::string &symbols, const std::vector<int> &nums, std::vector<std::vector<long>> &dp, int i, int j)
	{
		auto count_arrangements = [this](const std::string &symbols, const std::vector<int> &nums, std::vector<std::vector<long>> &dp, int i, int j) -> long
		{
			long result = 0;

			while (i < symbols.size() && symbols[i] == '.')
				++i;

			if (i >= symbols.size())
				return result + (j == nums.size());

			if (j == nums.size())
			{
				while (i < symbols.size() && (symbols[i] == '.' || symbols[i] == '?'))
					++i;
				return result + (i == symbols.size());
			}

			if (symbols[i] == '?')
				result += this->count_arrangements(symbols, nums, dp, i + 1, j);

			int count = 0;
			for (; i < symbols.size() && symbols[i] != '.' && count < nums[j]; ++i, ++count)
			{
			}
			if (count < nums[j]) // Not enough # or ? for the current count
				return result;

			if (i < symbols.size() && symbols[i] == '#') // Too many # for the current count
				return result;

			return result + this->count_arrangements(symbols, nums, dp, i + 1, j + 1);
		};

		if (dp[i][j] == -1)
		{
			dp[i][j] = count_arrangements(symbols, nums, dp, i, j);
		}
		return dp[i][j];
	}

	std::tuple<std::string, std::vector<int>, std::vector<std::vector<long>>> parse_line(std::ifstream &file, int num_repeat = 1)
	{
		assert(!file.eof() && "File reached EOF");
		std::string symbols;
		char c;
		for (c = file.get(); c != ' '; c = file.get())
		{
			symbols += c;
		}
		std::string repeated_symbols = symbols;
		for (int i = 1; i < num_repeat; ++i)
		{
			repeated_symbols += "?" + symbols;
		}
		symbols = repeated_symbols;

		// Ignore spaces
		for (c = file.get(); c == ' '; c = file.get())
		{
		}

		// Get nums
		std::vector<int> nums;
		int num = 0;
		for (; c != '\n' && c != EOF; c = file.get())
		{
			if (c == ',')
			{
				nums.push_back(num);
				num = 0;
				continue;
			}

			num = num * 10 + c - '0';
		}
		nums.push_back(num);

		nums.reserve(nums.size() * num_repeat);
		auto original_end = nums.end();
		for (int i = 1; i < num_repeat; ++i)
		{
			nums.insert(nums.end(), nums.begin(), original_end);
		}

		// Generate dp grid
		std::vector<std::vector<long>> dp(symbols.size() + 2, std::vector<long>(nums.size() + 1, -1));

		return std::make_tuple(symbols, nums, dp);
	}

	long generic_solve(int num_repeat = 1)
	{
		long result = 0;
		std::ifstream file;
		file.open(this->filepath);
		while (!file.eof())
		{
			auto [symbols, nums, dp] = this->parse_line(file, num_repeat);
			result += this->count_arrangements(symbols, nums, dp, 0, 0);
		}
		file.close();
		return result;
	}

	long part_1()
	{
		return this->generic_solve(1);
	}

	long part_2()
	{
		return this->generic_solve(5);
	}

public:
	Solver(std::filesystem::path filepath = "input.txt") : filepath(filepath) {}

	void solve()
	{
		std::cout << "Day 12" << std::endl;
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