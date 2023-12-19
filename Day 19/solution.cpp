#include <filesystem>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <functional>
#include <unordered_map>

typedef std::unordered_map<std::string, std::vector<std::tuple<char, char, int, std::string>>> Workflow;

class Solver
{
	std::filesystem::path filepath;

	static int variable_name_to_idx(char c)
	{
		switch (c)
		{
		case 'x':
			return 0;
		case 'm':
			return 1;
		case 'a':
			return 2;
		case 's':
			return 3;
		default:
			throw "Invalid variable name.";
		}
	};

	long generic_solve(const std::function<long(const Workflow &, std::ifstream &)> &solve)
	{
		std::ifstream file(this->filepath);
		Workflow workflow;
		for (char c = file.get(); c != '\n'; c = file.get())
		{
			std::string workflow_name;
			for (; c != '{'; c = file.get())
				workflow_name += c;

			// Get all the conditions
			while (c != '}')
			{
				c = file.get();

				// Get one condition
				bool colon_found = false;
				std::string variable_name, target;
				char operation = 0;
				int value = 0;
				for (; c != ',' && c != '}'; c = file.get())
				{
					if (c == '>' || c == '<')
					{
						operation = c;
					}
					else if (c == ':')
					{
						colon_found = true;
					}
					else if (colon_found)
					{
						target += c;
					}
					else if (operation != 0)
					{
						value = value * 10 + c - '0';
					}
					else
					{
						variable_name += c;
					}
				}

				if (operation == 0)
				{
					std::swap(target, variable_name);
				}

				workflow[workflow_name].emplace_back(variable_name[0], operation, value, target);
			}

			file.get(); // Ignore new line
		}

		return solve(workflow, file);
	}

	long part_1()
	{
		auto solve = [](const Workflow &workflow, std::ifstream &file) -> long
		{
			long total = 0;
			auto operations = [](int a, char operation, int b)
			{
				switch (operation)
				{
				case '>':
					return a > b;
				case '<':
					return a < b;
				default:
					throw "Invalid operation.";
				}
			};

			for (char c = file.get(); c != EOF; c = file.get())
			{
				int values[4];
				// Get all statements
				for (c = file.get(); c != '\n' && c != EOF; c = file.get())
				{
					// Get one statement
					int var_index = -1, value = 0;
					for (; c != ',' && c != '}'; c = file.get())
					{
						if (c == '=')
							continue;

						if (var_index == -1)
						{
							var_index = variable_name_to_idx(c);
							continue;
						}

						value = value * 10 + c - '0';
					}
					values[var_index] = value;
				}

				std::string current = "in";
				while (current != "A" && current != "R")
				{
					for (auto [variable_name,
										 operation,
										 value,
										 target] : workflow.at(current))
					{
						if ((operation == 0) ||
								operations(
										values[Solver::variable_name_to_idx(variable_name)],
										operation,
										value))
						{
							current = target;
							break;
						}
					}
				}

				if (current == "A")
				{
					for (int i = 0; i < 4; ++i)
					{
						total += values[i];
					}
				}
			}

			return total;
		};

		return this->generic_solve(solve);
	}

	long part_2()
	{
		auto get_new_ranges = [](
															int low,
															int high,
															int value,
															char operation) -> std::pair<std::pair<int, int>, std::pair<int, int>>
		{
			switch (operation)
			{
			case '<':
				return {{low, value - 1}, {value, high}};
			case '>':
				return {{value + 1, high}, {low, value}};
			default:
				throw "Invalid operation.";
			}
		};

		auto count_unique_arrangements = [get_new_ranges](
																				 auto self,
																				 const Workflow &workflow,
																				 const std::string &current,
																				 std::pair<int, int>(&ranges)[4]) -> long
		{
			if (current == "R")
				return 0;

			if (current == "A")
			{
				long result = 1;
				for (int i = 0; i < 4; ++i)
				{
					result *= ranges[i].second - ranges[i].first + 1;
				}
				return result;
			}

			long result = 0;
			std::pair<int, int> original_ranges[4];
			for (int i = 0; i < 4; ++i)
			{
				original_ranges[i] = ranges[i];
			}

			for (auto [variable_name, operation, value, target] : workflow.at(current))
			{
				if (operation == 0)
				{
					result += self(self, workflow, target, ranges);
					continue;
				}

				int variable_idx = Solver::variable_name_to_idx(variable_name);
				auto [low, high] = ranges[variable_idx];
				auto [positive_range, negative_range] = get_new_ranges(low, high, value, operation);
				if (positive_range.first > positive_range.second ||
						negative_range.first > negative_range.second)
					break;

				ranges[variable_idx] = positive_range;
				result += self(self, workflow, target, ranges);
				ranges[variable_idx] = negative_range;
			}

			for (int i = 0; i < 4; ++i)
			{
				ranges[i] = original_ranges[i];
			}
			return result;
		};

		auto solve = [count_unique_arrangements](
										 const Workflow &workflow,
										 std::ifstream &file) -> long
		{
			file.close();

			std::pair<int, int> ranges[4]{{1, 4000}, {1, 4000}, {1, 4000}, {1, 4000}};
			return count_unique_arrangements(count_unique_arrangements, workflow, "in", ranges);
		};
		return this->generic_solve(solve);
	}

public:
	Solver(std::filesystem::path filepath = "input.txt") : filepath(filepath) {}

	void solve()
	{
		std::cout << "Day 19" << std::endl;
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