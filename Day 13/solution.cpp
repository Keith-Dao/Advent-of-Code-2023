#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <cassert>
#include <functional>

typedef std::pair<int, int> coordinate;

class Solver
{
	std::filesystem::path filepath;

	std::vector<std::string> get_one_graph(std::ifstream &file)
	{
		assert(!file.eof() && "File reached EOF");

		std::vector<std::string> result;
		for (std::string line; std::getline(file, line) && line != "";)
		{
			result.push_back(line);
		}
		return result;
	}

	std::vector<std::string> transpose(const std::vector<std::string> &graph)
	{
		std::vector<std::string> result(graph.front().size());
		for (const std::string &row : graph)
		{
			for (int i = 0; i < row.size(); ++i)
			{
				result[i] += row[i];
			}
		}
		return result;
	}

	int generic_solve(std::function<bool(std::vector<std::string>, int)> is_valid_reflection)
	{
		int row_total = 0, col_total = 0;
		std::ifstream file(this->filepath);
		int count = 0;
		while (!file.eof())
		{
			std::vector<std::string> graph = this->get_one_graph(file);
			bool found_reflection = false;

			// Check vertical reflection
			for (int j = 0; j < graph.front().size() - 1; ++j)
			{
				if (is_valid_reflection(graph, j))
				{
					col_total += j + 1;
					found_reflection = true;
					++count;
					break;
				}
			}
			if (found_reflection)
				continue;

			// Check horizontal reflection
			graph = this->transpose(graph);
			for (int i = 0; i < graph.front().size() - 1; ++i)
			{
				if (is_valid_reflection(graph, i))
				{
					row_total += i + 1;
					++count;
					break;
				}
			}
		}
		file.close();
		return col_total + 100 * row_total;
	}

	int part_1()
	{
		auto is_valid_reflection = [](const std::vector<std::string> &reflection_sequences, int reflection_point) -> bool
		{
			for (const std::string &sequence : reflection_sequences)
			{
				for (int i = 0; i < std::min(reflection_point + 1, (int)sequence.size() - reflection_point - 1); ++i)
				{
					if (sequence[reflection_point - i] != sequence[reflection_point + i + 1])
					{
						return false;
					}
				}
			}
			return true;
		};
		return this->generic_solve(is_valid_reflection);
	}

	int part_2()
	{
		auto is_valid_reflection = [](const std::vector<std::string> &reflection_sequences, int reflection_point) -> bool
		{
			int count = 0;
			for (const std::string &sequence : reflection_sequences)
			{
				for (int i = 0; i < std::min(reflection_point + 1, (int)sequence.size() - reflection_point - 1); ++i)
				{
					if (sequence[reflection_point - i] == sequence[reflection_point + i + 1])
						continue;
					if (++count > 1)
						return false;
				}
			}
			return count == 1;
		};
		return this->generic_solve(is_valid_reflection);
	}

public:
	Solver(std::filesystem::path filepath = "input.txt") : filepath(filepath) {}

	void solve()
	{
		std::cout << "Day 13" << std::endl;
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