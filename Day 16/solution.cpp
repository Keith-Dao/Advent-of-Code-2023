#include <filesystem>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <utility>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <numeric>
#include <algorithm>
#include <execution>
#include <cassert>

constexpr auto max_size = std::numeric_limits<std::streamsize>::max();

typedef std::pair<int, int> coordinate;

// Gotten from: https://stackoverflow.com/a/16473277
struct pair_hash
{
	template <typename T, typename U>
	std::size_t operator()(const std::pair<T, U> &x) const
	{
		auto hash_combine = [](size_t &seed, T const &v)
		{
			seed ^= std::hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		};
		size_t result = std::hash<T>()(x.first);
		hash_combine(result, x.second);
		return result;
	}
};

enum symbols
{
	SPACE = '.',
	POSITIVE_MIRROR = '/',
	NEGATIVE_MIRROR = '\\',
	HORIZONTAL_SPLITTER = '-',
	VERTICAL_SPLITTER = '|'
};

class Solver
{
	std::filesystem::path filepath;

	std::vector<std::string> get_grid()
	{
		std::vector<std::string> result;
		std::ifstream file(this->filepath);
		std::string line;
		while (std::getline(file, line))
		{
			result.push_back(line);
		}
		file.close();
		return result;
	}

	int get_num_energized_positions(const std::vector<std::string> &grid, int i, int j, int d_i, int d_j)
	{
		auto add_next = [&grid = std::as_const(grid)](
												std::stack<std::tuple<int, int, int, int>> &stack,
												int i, int j, int d_i, int d_j)
		{
			switch (grid.at(i).at(j))
			{
			case SPACE:
				stack.emplace(i + d_i, j + d_j, d_i, d_j);
				break;
			case POSITIVE_MIRROR:
				stack.emplace(i - d_j, j - d_i, -d_j, -d_i);
				break;
			case NEGATIVE_MIRROR:
				stack.emplace(i + d_j, j + d_i, d_j, d_i);
				break;
			case VERTICAL_SPLITTER:
				if (d_i != 0)
				{
					stack.emplace(i + d_i, j + d_j, d_i, d_j);
					break;
				}
				stack.emplace(i + 1, j, 1, 0);
				stack.emplace(i - 1, j, -1, 0);
				break;
			case HORIZONTAL_SPLITTER:
				if (d_j != 0)
				{
					stack.emplace(i + d_i, j + d_j, d_i, d_j);
					break;
				}

				stack.emplace(i, j + 1, 0, 1);
				stack.emplace(i, j - 1, 0, -1);
				break;
			default:
				break;
			}
		};

		std::unordered_map<coordinate, std::unordered_set<coordinate, pair_hash>, pair_hash> energized;
		std::stack<std::tuple<int, int, int, int>> stack;
		stack.emplace(i, j, d_i, d_j);
		while (!stack.empty())
		{
			auto [i, j, d_i, d_j] = stack.top();
			stack.pop();

			if (0 > i || i >= grid.size() || 0 > j || j >= grid.front().size())
				continue;

			coordinate position{i, j}, direction{d_i, d_j};
			if (energized[position].contains(direction))
				continue;

			energized[position].insert(direction);
			add_next(stack, i, j, d_i, d_j);
		}

		return energized.size();
	}

	int part_1()
	{
		std::vector<std::string> grid = this->get_grid();
		return this->get_num_energized_positions(grid, 0, 0, 0, 1);
	}

	int part_2()
	{
		std::vector<std::string> grid = this->get_grid();

		int result = 0;
		std::mutex m;

		std::vector<int> range(grid.size());
		std::iota(range.begin(), range.end(), 0);
		std::for_each(
				std::execution::par, range.begin(), range.end(), [this, &grid, &result, &m](int i)
				{
					int current = std::max(
						this->get_num_energized_positions(grid, i, 0, 0, 1), this->get_num_energized_positions(grid, i, grid[i].size() - 1, 0, -1));
					std::lock_guard<std::mutex> guard(m);
					result = std::max(result, current); });

		range = std::vector<int>(grid.front().size());
		std::iota(range.begin(), range.end(), 0);
		std::for_each(
				std::execution::par, range.begin(), range.end(), [this, &grid, &result, &m](int j)
				{
					int current = std::max(this->get_num_energized_positions(grid, 0, j, 1, 0),
					this->get_num_energized_positions(grid, grid.size() - 1, j, -1, 0));
					std::lock_guard<std::mutex> guard(m);
					result = std::max(result, current); });
		return result;
	}

public:
	Solver(std::filesystem::path filepath = "input.txt") : filepath(filepath) {}

	void solve()
	{
		std::cout << "Day 16" << std::endl;
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