#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <functional>
#include <utility>

enum symbols
{
	SPACES = '.',
	ROUND_ROCKS = 'O',
	CUBED_ROCKS = '#'
};

typedef std::vector<std::vector<int>> row_list;

class Solver
{
	std::filesystem::path filepath;

	std::tuple<row_list, row_list, int> parse_file()
	{
		row_list round_rocks, cubed_rocks;
		int num_cols = 0, i = 0;
		std::ifstream file(this->filepath);
		while (!file.eof())
		{
			round_rocks.emplace_back();
			cubed_rocks.emplace_back();
			int j = 0;
			for (char c = file.get(); c != '\n' && c != EOF; c = file.get(), ++j)
			{
				if (c == SPACES)
					continue;

				if (c == ROUND_ROCKS)
				{
					round_rocks[i].push_back(j);
				}
				else if (c == CUBED_ROCKS)
				{
					cubed_rocks[i].push_back(j);
				}
				else
				{
					throw "Expected chars to be either '.', '#', or '0'.";
				}
			}
			num_cols = j;
			++i;
		}
		file.close();
		return std::make_tuple(round_rocks, cubed_rocks, num_cols);
	}

	row_list transpose_row_list(const row_list &rows, int new_length)
	{
		if (rows.empty())
			throw "Cannot transpose empty row list.";

		row_list result(new_length);
		for (int i = 0; i < rows.size(); ++i)
		{
			for (int j : rows[i])
				result[j].push_back(i);
		}

		return result;
	}

	row_list tilt(row_list &round_rocks, row_list &cubed_rocks, int num_cols, bool transpose, bool in_reverse)
	{
		int num_rows = round_rocks.size();
		row_list new_round_rocks(num_rows);
		std::function<void(int)> traverse;
		if (in_reverse)
		{
			traverse = [&round_rocks = std::as_const(round_rocks),
									&cubed_rocks = std::as_const(cubed_rocks),
									&new_round_rocks,
									transpose,
									num_rows,
									num_cols](int axis_value)
			{
				int i = cubed_rocks[axis_value].size() - 1,
						last = transpose ? num_rows : num_cols;
				std::vector<int> rounds = round_rocks[axis_value],
												 cubes = cubed_rocks[axis_value];

				for (auto it = rounds.rbegin(); it != rounds.rend(); ++it)
				{
					int round = *it;
					while (i >= 0 && cubes[i] > round)
					{
						last = cubes[i--];
					}
					--last;
					new_round_rocks[transpose ? last : axis_value].push_back(
							transpose ? axis_value : last);
				}
			};
		}
		else
		{
			traverse = [&round_rocks = std::as_const(round_rocks),
									&cubed_rocks = std::as_const(cubed_rocks),
									&new_round_rocks,
									transpose](int axis_value)
			{
				int i = 0, last = -1;
				std::vector<int> rounds = round_rocks[axis_value],
												 cubes = cubed_rocks[axis_value];
				for (int round : rounds)
				{
					while (i < cubes.size() && cubes[i] < round)
					{
						last = cubes[i++];
					}
					++last;

					new_round_rocks[transpose ? last : axis_value].push_back(
							transpose ? axis_value : last);
				}
			};
		}

		if (transpose)
		{
			round_rocks = this->transpose_row_list(round_rocks, num_cols);
			cubed_rocks = this->transpose_row_list(cubed_rocks, num_cols);
		}

		for (int axis_value = 0; axis_value < round_rocks.size(); ++axis_value)
		{
			traverse(axis_value);
		}

		if (transpose)
		{
			// Fixes the mutation from the initial transpose.
			round_rocks = this->transpose_row_list(round_rocks, num_cols);
			cubed_rocks = this->transpose_row_list(cubed_rocks, num_cols);
		}

		return new_round_rocks;
	}

	row_list tilt_north(row_list &round_rocks, row_list &cubed_rocks, int num_cols)
	{
		return this->tilt(round_rocks, cubed_rocks, num_cols, true, false);
	}

	row_list tilt_west(row_list &round_rocks, row_list &cubed_rocks, int num_cols)
	{
		return this->tilt(round_rocks, cubed_rocks, num_cols, false, false);
	}

	row_list tilt_south(row_list &round_rocks, row_list &cubed_rocks, int num_cols)
	{
		return this->tilt(round_rocks, cubed_rocks, num_cols, true, true);
	}

	row_list tilt_east(row_list &round_rocks, row_list &cubed_rocks, int num_cols)
	{
		return this->tilt(round_rocks, cubed_rocks, num_cols, false, true);
	}

	int calculate_north_load(const row_list &round_rocks)
	{
		int total = 0;
		for (int i = 0; i < round_rocks.size(); ++i)
		{
			total += round_rocks[i].size() * (round_rocks.size() - i);
		}
		return total;
	}

	std::string row_list_to_key(const row_list &graph)
	{
		std::string result = "";

		for (const std::vector<int> &row : graph)
		{
			result += "(";
			for (int x : row)
				result += std::to_string(x) + ",";

			result += ")";
		}
		return result;
	}

	int part_1()
	{
		auto [round_rocks, cubed_rocks, num_cols] = this->parse_file();
		round_rocks = this->tilt_north(round_rocks, cubed_rocks, num_cols);

		return this->calculate_north_load(round_rocks);
	}

	int part_2()
	{
		auto [round_rocks, cubed_rocks, num_cols] = this->parse_file();

		std::unordered_map<std::string, int> seen;
		std::vector<std::vector<std::vector<int>>> round_to_key;
		int cycle = -1, cycle_start = -1, target_cycles = 1000000000;

		for (int k = 0; k < target_cycles; ++k)
		{
			std::string key = this->row_list_to_key(round_rocks);
			if (seen.contains(key))
			{
				cycle_start = seen[key];
				cycle = k - cycle_start;
				break;
			}
			seen[key] = k;
			round_to_key.push_back(round_rocks);

			round_rocks = this->tilt_north(round_rocks, cubed_rocks, num_cols);
			round_rocks = this->tilt_west(round_rocks, cubed_rocks, num_cols);
			round_rocks = this->tilt_south(round_rocks, cubed_rocks, num_cols);
			round_rocks = this->tilt_east(round_rocks, cubed_rocks, num_cols);
		}

		if (cycle != -1 && cycle_start != -1)
		{
			round_rocks = round_to_key[cycle_start + (target_cycles - cycle_start) % cycle];
		}

		return this->calculate_north_load(round_rocks);
	}

public:
	Solver(std::filesystem::path filepath = "input.txt") : filepath(filepath) {}

	void solve()
	{
		std::cout << "Day 14" << std::endl;
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