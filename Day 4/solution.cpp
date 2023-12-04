#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <cassert>
#include <unordered_set>
#include <deque>

class Solver
{
	std::filesystem::path filepath;

	void skip_spaces(std::ifstream &file)
	{
		while (file.peek() == ' ')
			file.get();
	}

	std::pair<std::unordered_set<std::string>, std::vector<std::string>> parse_one_line(std::ifstream &file)
	{
		assert(!file.eof());

		// Discard everything in the line up to ':'
		for (char c = file.get(); c != EOF && c != ':'; c = file.get())
		{
		}

		// Winning numbers
		std::unordered_set<std::string> winning_nums;
		std::string num;
		for (char c = file.get(); c != EOF && c != '|'; c = file.get())
		{
			if (c == ' ')
			{
				if (!num.empty())
				{
					winning_nums.insert(num);
				}
				num.clear();
				continue;
			}

			num += c;
		}
		if (!num.empty())
			winning_nums.insert(num);

		// Gotten numbers
		std::vector<std::string> gotten_numbers;
		num.clear();
		for (char c = file.get(); c != EOF && c != '\n'; c = file.get())
		{
			if (c == ' ')
			{
				if (!num.empty())
				{
					gotten_numbers.push_back(num);
				}
				num.clear();
				continue;
			}

			num += c;
		}
		if (!num.empty())
			gotten_numbers.push_back(num);

		return std::make_pair(winning_nums, gotten_numbers);
	}

	int part_1()
	{
		std::ifstream file;
		file.open(this->filepath);

		int result = 0;
		while (!file.eof())
		{
			int line_score = 0;
			auto [winning_numbers, gotten_numbers] = this->parse_one_line(file);
			for (const std::string &num : gotten_numbers)
			{
				if (!winning_numbers.contains(num))
					continue;

				line_score = line_score == 0 ? 1 : line_score * 2;
			}
			result += line_score;
		}

		file.close();
		return result;
	}

	int part_2()
	{
		std::ifstream file;
		file.open(this->filepath);

		int result = 0;
		std::deque<int> extra_cards;
		while (!file.eof())
		{
			auto [winning_numbers, gotten_numbers] = this->parse_one_line(file);
			int current_number_cards = extra_cards.empty() ? 1 : extra_cards.front() + 1;
			if (!extra_cards.empty())
				extra_cards.pop_front();

			int number_matching = 0;
			for (const std::string &num : gotten_numbers)
			{
				if (!winning_numbers.contains(num))
					continue;

				++number_matching;
			}

			for (int i = 0; i < number_matching; ++i)
			{
				if (extra_cards.size() == i)
				{
					extra_cards.push_back(current_number_cards);
				}
				else
				{
					extra_cards[i] += current_number_cards;
				}
			}
			result += number_matching * current_number_cards + 1;
		}

		file.close();
		return result;
	}

public:
	Solver(std::filesystem::path filepath = "input.txt") : filepath(filepath) {}

	void solve()
	{
		std::cout << "Day 4" << std::endl;
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