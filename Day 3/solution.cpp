#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <functional>
#include <unordered_set>

typedef std::pair<int, int> Coordinate;

struct coordinate_hash
{
	template <class T1, class T2>
	std::size_t operator()(const std::pair<T1, T2> &pair) const
	{
		return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
	}
};

class Solver
{
	std::filesystem::path filepath;

	std::tuple<std::vector<Coordinate>, std::unordered_map<Coordinate, int, coordinate_hash>, std::vector<std::vector<Coordinate>>, std::vector<int>> get_nums_and_symbols(std::function<bool(char)> is_valid_symbol)
	{
		std::vector<Coordinate> symbols;
		std::unordered_map<Coordinate, int, coordinate_hash> positions_to_id;
		std::vector<std::vector<Coordinate>> id_to_positions;
		std::vector<int> id_to_num;

		std::ifstream file;
		file.open(this->filepath);

		for (int i = 0; !file.eof(); ++i) // Iterate over each line
		{
			int num = -1, num_digits = 0, n = 0;

			for (int j = 0, c; (c = file.get()) != '\n' && c != EOF; ++j, ++n) // Iterate over each character
			{
				if (std::isdigit(c))
				{
					if (num == -1)
						num = 0;
					num = num * 10 + c - '0';
					++num_digits;
					continue;
				}

				if (num != -1)
				{
					id_to_positions.emplace_back();
					for (int d_j = 1; d_j <= num_digits; ++d_j)
					{
						Coordinate positions{i, j - d_j};
						positions_to_id[positions] = id_to_num.size();
						id_to_positions.back().push_back(positions);
					}
					id_to_num.push_back(num);
				}

				num = -1;
				num_digits = 0;

				if (is_valid_symbol(c))
					symbols.emplace_back(i, j);
			}

			if (num != -1)
			{
				id_to_positions.emplace_back();
				for (int d_j = 1; d_j <= num_digits; ++d_j)
				{
					Coordinate positions{i, n - 1 - d_j};
					positions_to_id[positions] = id_to_num.size();
					id_to_positions.back().push_back(positions);
				}
				id_to_num.push_back(num);
			}
		}

		file.close();

		return std::make_tuple(symbols, positions_to_id, id_to_positions, id_to_num);
	}

	Coordinate directions[8]{{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};

	template <typename Iterator>
	int id_to_num_product(std::vector<int> id_to_num, Iterator begin, Iterator end)
	{
		int result = 1;
		while (begin != end)
		{
			result *= id_to_num[*begin];
			++begin;
		}
		return result;
	}

	int part_1()
	{
		auto [symbols,
					positions_to_id,
					id_to_positions,
					id_to_num] = this->get_nums_and_symbols([](char c)
																									{ return c != '.' && c != ' '; });

		int result = 0;
		for (auto [i, j] : symbols)
		{
			for (auto [d_i, d_j] : this->directions)
			{
				Coordinate adjacent_point{i + d_i, j + d_j};

				if (!positions_to_id.contains(adjacent_point))
					continue;

				int num_id = positions_to_id[adjacent_point],
						num = id_to_num[num_id];
				result += num;

				for (Coordinate position : id_to_positions[num_id])
				{
					positions_to_id.erase(position);
				}
			}
		}
		return result;
	}

	int part_2()
	{
		auto [symbols,
					positions_to_id,
					_,
					id_to_num] = this->get_nums_and_symbols([](char c)
																									{ return c != '.' && c != ' '; });

		int result = 0;
		for (auto [i, j] : symbols)
		{
			std::unordered_set<int> seen;
			for (auto [d_i, d_j] : this->directions)
			{
				Coordinate adjacent_point{i + d_i, j + d_j};

				if (!positions_to_id.contains(adjacent_point))
					continue;

				seen.insert(positions_to_id[adjacent_point]);
				if (seen.size() > 2)
				{
					break;
				}
			}

			if (seen.size() == 2)
			{
				result += this->id_to_num_product(id_to_num, seen.begin(), seen.end());
			}
		}
		return result;
	}

public:
	Solver(std::filesystem::path filepath = "input.txt") : filepath(filepath) {}

	void solve()
	{
		std::cout << "Day 3" << std::endl;
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