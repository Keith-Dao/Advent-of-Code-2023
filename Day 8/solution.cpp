#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <functional>

constexpr auto max_size = std::numeric_limits<std::streamsize>::max();

class Solver
{
	std::filesystem::path filepath;

	std::pair<std::string, std::unordered_map<std::string, std::pair<std::string, std::string>>> parse_order_and_graph()
	{
		std::ifstream file;
		file.open(this->filepath);

		char c;
		// Get order
		std::string order;
		while ((c = file.get()) != '\n')
		{
			order += c;
		}

		// Blank line
		file.ignore(max_size, '\n');

		// Graph
		std::unordered_map<std::string, std::pair<std::string, std::string>> graph;
		while (file.peek() != EOF)
		{
			// Source node
			std::string node, left, right;
			for (c = file.get(); c != ' '; c = file.get())
			{
				node += c;
			}

			// Skip to left node
			file.ignore(max_size, '(');

			// Left node
			for (c = file.get(); c != ','; c = file.get())
			{
				left += c;
			}

			// Skip to right node
			for (c = file.get(); c == ' '; c = file.get())
			{
			}

			// Right node
			for (; c != ')'; c = file.get())
			{
				right += c;
			}

			// Skip to next line
			file.ignore(max_size, '\n');

			graph[node] = std::make_pair(left, right);
		}
		file.close();

		return std::make_pair(order, graph);
	}

	long get_steps(const std::string &order, std::string node, const std::unordered_map<std::string, std::pair<std::string, std::string>> &graph, std::function<bool(std::string)> is_end_node)
	{
		long steps = 0;
		while (!is_end_node(node))
		{
			char direction = order[steps++ % order.size()];
			node = direction == 'L' ? graph.at(node).first : graph.at(node).second;
		}
		return steps;
	}

	long gcd(long a, long b)
	{
		if (a < b)
			std::swap(a, b);

		while (b != 0)
		{
			long temp = a % b;
			a = b;
			b = temp;
		}
		return a;
	}

	long lcm(std::vector<long> nums)
	{
		while (nums.size() > 1)
		{
			long a = nums.back(), b;
			nums.pop_back();
			b = nums.back();
			nums.pop_back();
			nums.push_back(a / this->gcd(a, b) * b);
		}
		return nums.front();
	}

	long part_1()
	{
		auto [order, graph] = this->parse_order_and_graph();

		std::string start_node = "AAA";
		if (!graph.contains(start_node))
			return -1;

		return this->get_steps(order, start_node, graph, [](const std::string &node)
													 { return node == "ZZZ"; });
	}

	long part_2()
	{
		auto [order, graph] = this->parse_order_and_graph();
		std::vector<long> cycles;
		for (auto [node, _] : graph)
		{
			if (node.back() != 'A')
				continue;
			cycles.push_back(this->get_steps(order, node, graph, [](const std::string &node)
																			 { return node.back() == 'Z'; }));
		}

		return this->lcm(cycles);
	}

public:
	Solver(std::filesystem::path filepath = "input.txt") : filepath(filepath) {}

	void solve()
	{
		std::cout << "Day 8" << std::endl;
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