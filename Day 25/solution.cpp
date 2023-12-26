#include <filesystem>
#include <sstream>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <queue>
#include <climits>

class Solver
{
	std::filesystem::path filepath;

	static bool bfs(
			std::unordered_map<std::string, std::unordered_map<std::string, int>> &graph,
			std::unordered_map<std::string, std::string> &parents,
			const std::string &source,
			const std::string &target)
	{
		for (auto &[node, _] : parents)
		{
			parents[node] = "";
		}

		parents[source] = source;
		std::queue<std::string> queue;
		queue.push(source);
		while (!queue.empty())
		{
			std::string node = queue.front();
			queue.pop();
			for (auto [next, capacity] : graph[node])
			{
				if (capacity <= 0 || parents[next] != "")
					continue;

				parents[next] = node;
				queue.push(next);
			}
		}

		return parents[target] != "";
	}

	static int min_cuts(
			std::unordered_map<std::string, std::unordered_map<std::string, int>> &graph,
			std::unordered_map<std::string, std::string> &parents,
			const std::string &source,
			const std::string &target)
	{
		for (auto &[_, neighbours] : graph)
		{
			for (auto &[next, _] : neighbours)
				neighbours[next] = 1;
		}

		int max_flow = 0;
		while (Solver::bfs(graph, parents, source, target))
		{
			int flow = INT_MAX;
			std::string node = target;
			while (node != source)
			{
				flow = std::min(flow, graph[parents[node]][node]);
				node = parents[node];
			}

			max_flow += flow;
			node = target;
			while (node != source)
			{
				std::string prev = parents[node];
				graph[prev][node] -= flow;
				graph[node][prev] += flow;
				node = prev;
			}
		}

		return max_flow;
	}

	int part_1()
	{
		std::unordered_map<std::string, std::unordered_map<std::string, int>> graph;
		{
			std::ifstream file(this->filepath);
			for (char c = file.get(); c != EOF; c = file.get())
			{
				std::string source;
				for (; c != ':'; c = file.get())
				{
					source += c;
				}

				c = file.get();
				std::string target;
				for (c = file.get(); c != '\n' && c != EOF; c = file.get())
				{
					if (c == ' ')
					{
						graph[source][target] = 1;
						graph[target][source] = 1;
						target.clear();
						continue;
					}

					target += c;
				}
				graph[source][target] = 1;
				graph[target][source] = 1;
			}
		}

		std::unordered_map<std::string, std::string> parents;
		for (auto [node, _] : graph)
		{
			parents[node] = "";
		}
		auto [source, _] = *graph.begin();
		for (auto &[target, _] : graph)
		{
			if (source == target)
				continue;

			if (this->min_cuts(graph, parents, source, target) == 3)
				break;
		}

		int component_1_size = 0;
		for (auto [_, parent] : parents)
		{
			component_1_size += parent != "";
		}
		return (graph.size() - component_1_size) * component_1_size;
	}

	long part_2()
	{
		return -1;
	}

public:
	Solver(std::filesystem::path filepath = "input.txt") : filepath(filepath) {}

	void solve()
	{
		std::cout << "Day 25" << std::endl;
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