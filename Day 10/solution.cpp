#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <algorithm>

typedef std::pair<int, int> coordinate;
enum Direction
{
	NORTH = 0,
	SOUTH = 1,
	EAST = 2,
	WEST = 3
};

std::pair<int, int> direction_values[4]{{-1, 0}, {1, 0}, {0, 1}, {0, -1}};

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

typedef std::unordered_map<coordinate, std::vector<coordinate>, pair_hash> graph;

class Solver
{
	std::filesystem::path filepath;

	std::tuple<graph, coordinate, std::unordered_set<coordinate, pair_hash>, int, int> generate_graph()
	{
		graph graph;
		std::unordered_map<char, std::vector<coordinate>> pipes{
				{'|', {direction_values[SOUTH], direction_values[NORTH]}},
				{'-', {direction_values[EAST], direction_values[WEST]}},
				{'L', {direction_values[NORTH], direction_values[EAST]}},
				{'J', {direction_values[NORTH], direction_values[WEST]}},
				{'7', {direction_values[SOUTH], direction_values[WEST]}},
				{'F', {direction_values[SOUTH], direction_values[EAST]}},
		};
		std::unordered_set<coordinate, pair_hash> spaces;
		int m = 0, n;
		coordinate start;

		std::ifstream file;
		file.open(this->filepath);
		for (; !file.eof(); ++m)
		{
			int j = 0;
			for (char c = file.get(); c != '\n' && c != EOF; c = file.get(), ++j)
			{
				switch (c)
				{
				case '.':
					spaces.emplace(m, j);
					break;
				case 'S':
					start = std::make_pair(m, j);
					break;
				default:
					graph[{m, j}] = pipes[c];
				}
				n = j + 1;
			}
		}
		file.close();

		// Determine start pipe directions
		auto [i, j] = start;
		for (auto [d_i, d_j] : direction_values)
		{
			coordinate next_node{i + d_i, j + d_j};
			if (!graph.contains(next_node))
				continue;

			coordinate opposite_direction{-d_i, -d_j};
			for (auto direction : graph[next_node])
			{
				if (direction != opposite_direction)
					continue;
				graph[start].emplace_back(d_i, d_j);
				break;
			}
		}

		return std::make_tuple(graph, start, spaces, m, n);
	}

	std::pair<int, std::unordered_set<coordinate, pair_hash>> traverse_loop(const graph &graph, const coordinate &start)
	{
		std::queue<coordinate> queue;
		std::unordered_set<coordinate, pair_hash> loop_nodes;
		int steps = 0;

		queue.push(start);
		loop_nodes.insert(start);

		while (!queue.empty())
		{
			int queue_size = queue.size();
			for (int k = 0; k < queue_size; ++k)
			{
				coordinate node = queue.front();
				queue.pop();
				for (auto [d_i, d_j] : graph.at(node))
				{
					coordinate next_node{node.first + d_i, node.second + d_j};
					if (!loop_nodes.contains(next_node) && graph.contains(next_node))
					{
						queue.push(next_node);
						loop_nodes.insert(next_node);
					}
				}
			}
			++steps;
		}

		return std::make_pair(steps - 1, loop_nodes);
	}

	void flood_mark_outside_nodes(coordinate node, std::unordered_set<coordinate, pair_hash> &visited, std::unordered_set<coordinate, pair_hash> &spaces, int m, int n)
	{
		if (visited.contains(node))
			return;
		visited.insert(node);

		std::stack<coordinate> stack;
		stack.push(node);
		while (!stack.empty())
		{
			auto [i, j] = stack.top();
			stack.pop();
			if (0 > i || i >= 3 * m || 0 > j || j >= 3 * n)
				continue;

			if (i % 3 == 1 && j % 3 == 1)
				spaces.erase({i / 3, j / 3});

			for (auto [d_i, d_j] : direction_values)
			{
				coordinate next_node{i + d_i, j + d_j};
				if (!visited.contains(next_node))
				{
					stack.push(next_node);
					visited.insert(next_node);
				}
			}
		}
	}

	int part_1()
	{
		graph graph;
		coordinate start;
		std::tie(graph, start, std::ignore, std::ignore, std::ignore) = this->generate_graph();

		return this->traverse_loop(graph, start).first;
	}

	int part_2()
	{
		auto [graph, start, spaces, m, n] = this->generate_graph();
		auto [_, loop_nodes] = this->traverse_loop(graph, start);

		// Consider all pipes not in the loop as a space
		for (auto [node, _] : graph)
		{
			if (loop_nodes.contains(node))
				continue;
			spaces.insert(node);
		}

		// Enlarge map so a 1x1 element is 3x3
		std::unordered_set<coordinate, pair_hash> visited;
		for (auto node : loop_nodes)
		{
			for (auto [d_i, d_j] : graph[node])
			{
				visited.emplace(node.first * 3 + 1 + d_i, node.second * 3 + 1 + d_j);
			}
			visited.emplace(node.first * 3 + 1, node.second * 3 + 1);
		}

		for (int i = 1; i < 3 * m; i += 3)
		{
			this->flood_mark_outside_nodes({i, 1}, visited, spaces, m, n);
			this->flood_mark_outside_nodes({i, (n - 1) * 3 + 1}, visited, spaces, m, n);
		}
		for (int j = 1; j < 3 * m; j += 3)
		{
			this->flood_mark_outside_nodes({1, j}, visited, spaces, m, n);
			this->flood_mark_outside_nodes({(m - 1) * 3 + 1, j}, visited, spaces, m, n);
		}

		return spaces.size();
	}

public:
	Solver(std::filesystem::path filepath = "input.txt") : filepath(filepath) {}

	void solve()
	{
		std::cout << "Day 10" << std::endl;
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