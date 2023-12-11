#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_set>

typedef std::pair<int, int> coordinate;

class Solver
{
	std::filesystem::path filepath;

	int distance(int a, int b, const std::vector<int> &empty_positions, int expansion_factor)
	{
		int max_index = std::upper_bound(empty_positions.begin(), empty_positions.end(), std::max(a, b)) - empty_positions.begin(),
				min_index = std::lower_bound(empty_positions.begin(), empty_positions.end(), std::min(a, b)) - empty_positions.begin();
		return std::abs(a - b) + (max_index - min_index) * (expansion_factor - 1);
	}

	int distance(coordinate galaxy_1, coordinate galaxy_2, const std::pair<std::vector<int>, std::vector<int>> &empty_axis_positions, int expansion_factor)
	{
		return this->distance(galaxy_1.first, galaxy_2.first, empty_axis_positions.first, expansion_factor) + this->distance(galaxy_1.second, galaxy_2.second, empty_axis_positions.second, expansion_factor);
	}

	long generic_solve(int expansion_factor)
	{
		std::vector<coordinate> galaxies;
		int m = 0, n;

		std::ifstream file;
		file.open(this->filepath);
		while (!file.eof())
		{
			n = 0;
			for (char c = file.get(); c != '\n' && c != EOF; c = file.get(), ++n)
			{
				if (c == '#')
					galaxies.emplace_back(m, n);
			}
			++m;
		}
		file.close();

		std::unordered_set<int> non_empty_rows, non_empty_cols;
		for (auto [i, j] : galaxies)
		{
			non_empty_rows.insert(i);
			non_empty_cols.insert(j);
		}

		std::pair<std::vector<int>, std::vector<int>> empty_axis_positions;
		for (int i = 0; i < m; ++i)
		{
			if (!non_empty_rows.contains(i))
				empty_axis_positions.first.push_back(i);
		}
		for (int j = 0; j < n; ++j)
		{
			if (!non_empty_cols.contains(j))
				empty_axis_positions.second.push_back(j);
		}

		long total = 0;
		for (int i = 0; i < galaxies.size(); ++i)
		{
			for (int j = 0; j < i; ++j)
			{
				total += this->distance(galaxies[i], galaxies[j], empty_axis_positions, expansion_factor);
			}
		}
		return total;
	}

	long part_1()
	{
		return this->generic_solve(2);
	}

	long part_2()
	{
		return this->generic_solve(1000000);
	}

public:
	Solver(std::filesystem::path filepath = "input.txt") : filepath(filepath) {}

	void solve()
	{
		std::cout << "Day 11" << std::endl;
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