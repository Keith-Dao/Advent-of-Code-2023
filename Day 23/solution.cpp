#include <filesystem>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <stack>

// Gotten from: https://stackoverflow.com/a/16473277
struct pair_hash
{
	template <typename T, typename U>
	std::size_t operator()(const std::pair<T, U> &x) const
	{
		auto hash_combine = [](size_t &seed, U const &v)
		{
			seed ^= std::hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		};
		size_t result = std::hash<T>()(x.first);
		hash_combine(result, x.second);
		return result;
	}
};

template <typename V>
using pair_map = std::unordered_map<std::pair<int, int>, V, pair_hash>;

class Solver
{
	std::filesystem::path filepath;

	std::pair<
			pair_map<pair_map<int>>, std::pair<int, int>>
	get_graph(bool ignore_slopes)
	{
		std::vector<std::string> grid;
		std::ifstream file(this->filepath);
		for (std::string line; std::getline(file, line);)
			grid.push_back(line);

		int m = grid.size(), n = grid.front().size();
		std::string slopes = "<^>v";
		int dir[5]{0, -1, 0, 1, 0};
		pair_map<pair_map<int>> graph;
		for (int i = 0; i < m; ++i)
		{
			for (int j = 0; j < n; ++j)
			{
				if (grid[i][j] == '#')
					continue;

				for (int k = 0; k < 4; ++k)
				{
					if (!ignore_slopes &&
							grid[i][j] != '.' &&
							grid[i][j] != '#' &&
							grid[i][j] != slopes[k])
						continue;

					int n_i = i + dir[k], n_j = j + dir[k + 1];
					if (0 > n_i ||
							n_i >= m ||
							0 > n_j ||
							n_j >= n ||
							grid[n_i][n_j] == '#')
						continue;
					graph[{i, j}][{n_i, n_j}] = 1;
				}
			}
		}

		bool done = false;
		while (!done)
		{
			done = true;
			for (auto [node, neighbours] : graph)
			{
				if (neighbours.size() != 2)
					continue;

				auto [a, a_steps] = *neighbours.begin();
				auto [b, b_steps] = *(++neighbours.begin());

				if (graph[a].contains(node))
				{
					graph[a].erase(node);
					graph[a][b] = a_steps + b_steps;
				}

				if (graph[b].contains(node))
				{
					graph[b].erase(node);
					graph[b][a] = a_steps + b_steps;
				}

				graph.erase(node);
				done = false;
				break;
			}
		}

		return {graph, {m, n}};
	}

	int generic_solve(bool ignore_slopes)
	{
		auto [graph, dimensions] = this->get_graph(ignore_slopes);
		auto [m, n] = dimensions;

		std::stack<std::tuple<int, int, int>> stack;
		stack.emplace(0, 1, 0);
		std::vector<std::vector<bool>> visited(m, std::vector<bool>(n, false));
		int result = 0;
		while (!stack.empty())
		{
			auto [i, j, total] = stack.top();
			stack.pop();
			if (total == -1)
			{
				visited[i][j] = false;
				continue;
			}

			if (i == m - 1)
			{
				result = std::max(result, total);
				continue;
			}

			visited[i][j] = true;
			stack.emplace(i, j, -1);
			for (auto [next, cost] : graph[{i, j}])
			{
				auto [n_i, n_j] = next;
				if (visited[n_i][n_j])
					continue;

				stack.emplace(n_i, n_j, total + cost);
			}
		}
		return result;
	}

	int part_1()
	{
		return this->generic_solve(false);
	}

	int part_2()
	{
		return this->generic_solve(true);
	}

public:
	Solver(std::filesystem::path filepath = "input.txt") : filepath(filepath) {}

	void solve()
	{
		std::cout << "Day 23" << std::endl;
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