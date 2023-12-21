#include <filesystem>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_set>

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

const int dir[5]{0, -1, 0, 1, 0};

class Solver
{
	std::filesystem::path filepath;

	std::pair<std::vector<std::string>, std::pair<int, int>> get_graph()
	{
		std::ifstream file(this->filepath);
		std::vector<std::string> graph;
		std::pair<int, int> start;
		int i = 0;
		for (char c = file.get(); c != EOF; c = file.get(), ++i)
		{
			std::string line;
			for (int j = 0; c != '\n' && c != EOF; ++j, c = file.get())
			{
				line += c;
				if (c == 'S')
					start = std::make_pair(i, j);
			}
			graph.push_back(line);
		}

		return std::make_pair(graph, start);
	}

	static void step(const std::vector<std::string> &graph, std::queue<std::pair<int, int>> &queue, std::unordered_set<std::pair<int, int>, pair_hash> &visited)
	{
		visited.clear();

		for (int queue_length = queue.size(); queue_length > 0; --queue_length)
		{
			auto [i, j] = queue.front();
			queue.pop();

			for (int k = 0; k < 4; ++k)
			{
				int n_i = i + dir[k], n_j = j + dir[k + 1];
				if (0 > n_i ||
						n_i >= graph.size() ||
						0 > n_j ||
						n_j >= graph.front().size() ||
						graph[n_i][n_j] == '#' ||
						visited.contains({n_i, n_j}))
					continue;

				visited.emplace(n_i, n_j);
				queue.emplace(n_i, n_j);
			}
		}
	}

	long part_1()
	{
		auto [graph, start] = this->get_graph();

		std::queue<std::pair<int, int>> queue;
		queue.push(start);
		std::unordered_set<std::pair<int, int>, pair_hash> visited;

		for (int i = 0; i < 64; ++i)
			this->step(graph, queue, visited);
		return visited.size();
	}

	long part_2()
	{
		auto [graph, start] = this->get_graph();

		int m = graph.size(), n = graph.front().size(), target_step = 26501365;

		// See https://www.reddit.com/r/adventofcode/comments/18nevo3/comment/keam21w
		// for a better explanation
		// Find all positions that can be reached
		std::queue<std::pair<int, int>> queue;
		queue.push(start);
		std::unordered_set<std::pair<int, int>, pair_hash> reachable;
		reachable.insert(start);
		while (!queue.empty())
		{
			auto [i, j] = queue.front();
			queue.pop();
			for (int k = 0; k < 4; ++k)
			{
				int n_i = i + dir[k], n_j = j + dir[k + 1];
				if (0 > n_i ||
						n_i >= graph.size() ||
						0 > n_j ||
						n_j >= graph.front().size() ||
						graph[n_i][n_j] == '#' || reachable.contains({n_i, n_j}))
					continue;
				reachable.emplace(n_i, n_j);
				queue.emplace(n_i, n_j);
			}
		}

		// Find the inner diamond positions
		queue = std::queue<std::pair<int, int>>();
		queue.push(start);
		std::unordered_set<std::pair<int, int>, pair_hash> visited,
				inner_odd_positions,
				inner_even_positions,
				outer_positions;
		for (int i = 0; i < target_step % m; ++i)
			this->step(graph, queue, visited);
		inner_odd_positions = std::unordered_set<std::pair<int, int>, pair_hash>(visited);
		this->step(graph, queue, visited);
		inner_even_positions = std::unordered_set<std::pair<int, int>, pair_hash>(visited);

		outer_positions = std::unordered_set<std::pair<int, int>, pair_hash>(reachable);
		for (const std::pair<int, int> &position : inner_even_positions)
		{
			outer_positions.erase(position);
		}
		for (const std::pair<int, int> &position : inner_odd_positions)
		{
			outer_positions.erase(position);
		}

		long num_repeats = 2 * target_step / m + 1, // Number of tiles along an axis
				num_even_tiles = num_repeats / 2,
				 num_odd_tiles = num_repeats - num_repeats / 2;
		if (num_even_tiles > num_odd_tiles)
			std::swap(num_even_tiles, num_odd_tiles);

		return (
				num_odd_tiles * num_odd_tiles * inner_odd_positions.size() + num_odd_tiles * num_even_tiles * outer_positions.size() + num_even_tiles * num_even_tiles * inner_even_positions.size());
	}

public:
	Solver(std::filesystem::path filepath = "input.txt") : filepath(filepath) {}

	void solve()
	{
		std::cout << "Day 21" << std::endl;
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