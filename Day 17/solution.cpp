#include <filesystem>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>

typedef std::tuple<int, int, int> coordinate_direction;

template <typename T>
class min_heap
{
	std::vector<T> heap;

public:
	bool empty()
	{
		return this->heap.empty();
	}

	T front()
	{
		return this->heap.front();
	}

	void push(const T &val)
	{
		this->heap.push_back(val);
		std::push_heap(this->heap.begin(), this->heap.end(), std::greater<T>());
	}

	template <class... Args>
	void emplace(Args &&...args)
	{
		this->heap.emplace_back(args...);
	}

	void pop()
	{
		std::pop_heap(this->heap.begin(), this->heap.end(), std::greater<T>());
		this->heap.pop_back();
	}
};

class Solver
{
	std::filesystem::path filepath;

	std::vector<std::vector<int>> get_graph()
	{
		std::vector<std::vector<int>> result;
		std::ifstream file(this->filepath);
		while (!file.eof())
		{
			result.emplace_back();
			for (char c = file.get(); c != EOF && c != '\n'; c = file.get())
			{
				result.back().push_back(c - '0');
			}
		}
		file.close();
		return result;
	}

	int generic_solve(const std::vector<std::vector<int>> &graph, int max_steps_without_turning, int min_steps = 0)
	{
		int m = graph.size(), n = graph.front().size();
		std::vector<std::vector<std::vector<int>>> visited(
				m, std::vector<std::vector<int>>(n, std::vector<int>(2, -1)));
		min_heap<std::pair<int, coordinate_direction>> heap;
		for (int axis = 0; axis < 2; ++axis)
		{
			coordinate_direction state{0, 0, axis};
			heap.emplace(0, state);
		}

		while (!heap.empty())
		{
			auto [cost, state] = heap.front();
			heap.pop();
			auto [i, j, axis] = state;
			if (i == m - 1 && j == n - 1)
				return cost;

			for (int change : {-1, 1})
			{
				int next_cost = cost, next_i = i, next_j = j;
				for (int steps = 1; steps <= max_steps_without_turning; ++steps)
				{
					next_i += (axis == 0) * change;
					next_j += (axis == 1) * change;

					if (0 > next_i || next_i >= m || 0 > next_j || next_j >= n)
						break;
					next_cost += graph[next_i][next_j];
					if (steps < min_steps)
						continue;

					coordinate_direction next_state{next_i, next_j, 1 - axis};
					if (visited[next_i][next_j][1 - axis] != -1 && visited[next_i][next_j][1 - axis] <= next_cost)
						continue;
					visited[next_i][next_j][1 - axis] = next_cost;
					heap.emplace(next_cost, next_state);
				}
			}
		}
		return -1;
	}

	int part_1()
	{
		return this->generic_solve(this->get_graph(), 3);
	}

	int part_2()
	{
		return this->generic_solve(this->get_graph(), 10, 4);
	}

public:
	Solver(std::filesystem::path filepath = "input.txt") : filepath(filepath) {}

	void solve()
	{
		std::cout << "Day 17" << std::endl;
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