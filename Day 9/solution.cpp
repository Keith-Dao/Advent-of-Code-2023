#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <stack>

class Solver
{
	std::filesystem::path filepath;

	bool is_constant(const std::vector<int> &sequence)
	{
		int num = sequence.front();
		for (int x : sequence)
		{
			if (x != num)
				return false;
		}
		return true;
	}

	int get_previous_sequence_nums(int num, std::stack<std::vector<int>> &sequences, bool find_last)
	{
		std::vector<int> nums = sequences.top();
		sequences.pop();
		if (find_last)
			return num + nums.end()[-1];
		return nums[0] - num;
	}

	int get_num(std::ifstream &file, bool find_last)
	{
		std::vector<int> nums;
		int num = 0;
		bool negative = false;
		for (char c = file.get(); c != '\n' && c != EOF; c = file.get())
		{
			if (c == ' ')
			{
				if (negative)
				{
					num = -num;
					negative = false;
				}
				nums.push_back(num);
				num = 0;
				continue;
			}
			else if (c == '-')
			{
				negative = true;
				continue;
			}

			num = num * 10 + c - '0';
		}
		if (negative)
		{
			num = -num;
		}
		nums.push_back(num);

		std::stack<std::vector<int>> sequences;
		while (!this->is_constant(nums))
		{
			sequences.push(nums);
			std::vector<int> temp(nums.size() - 1);
			for (int i = 0; i < nums.size() - 1; ++i)
			{
				temp[i] = nums[i + 1] - nums[i];
			}
			nums = temp;
		}

		num = nums[find_last ? nums.size() - 1 : 0];
		while (!sequences.empty())
		{
			num = this->get_previous_sequence_nums(num, sequences, find_last);
		}
		return num;
	}

	int generic_solve(bool find_last)
	{
		std::ifstream file;
		file.open(this->filepath);
		int result = 0;
		while (!file.eof())
		{
			result += this->get_num(file, find_last);
		}
		file.close();
		return result;
	}

	int part_1()
	{
		return this->generic_solve(true);
	}

	int part_2()
	{
		return this->generic_solve(false);
	}

public:
	Solver(std::filesystem::path filepath = "input.txt") : filepath(filepath) {}

	void solve()
	{
		std::cout << "Day 9" << std::endl;
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