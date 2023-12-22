#include <filesystem>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <stack>

// Gets the first item in a tuple.
template <typename T, typename... Ts>
auto head(std::tuple<T, Ts...> t)
{
	return std::get<0>(t);
}

// Creates a new tuple excluding the first item.
template <std::size_t... Ns, typename... Ts>
auto tail_impl(std::index_sequence<Ns...>, std::tuple<Ts...> t)
{
	return std::make_tuple(std::get<Ns + 1u>(t)...);
}

template <typename... Ts>
auto tail(std::tuple<Ts...> t)
{
	return tail_impl(std::make_index_sequence<sizeof...(Ts) - 1u>(), t);
}

// Recursive nested unordered map
template <typename V, typename... Ts>
class nested_unordered_map;

template <typename V, typename T>
class nested_unordered_map<std::tuple<T>, V>
{
	std::unordered_map<T, V> data;

public:
	V &operator[](const std::tuple<T> &key)
	{
		return this->data[head(key)];
	}

	V operator[](const std::tuple<T> &key) const
	{
		return this->data.at(head(key));
	}

	bool contains(const T &key) const
	{
		return this->data.contains(key);
	}
};

template <typename V, typename T, typename... Ts>
class nested_unordered_map<std::tuple<T, Ts...>, V>
{
	std::unordered_map<T, nested_unordered_map<std::tuple<Ts...>, V>> data;

public:
	V &operator[](const std::tuple<T, Ts...> &key)
	{
		return this->data[head(key)][tail(key)];
	}

	V operator[](const std::tuple<T, Ts...> &key) const
	{
		return this->data.at(head(key))[tail(key)];
	}

	bool contains(const T &key, const Ts &...rest) const
	{
		return this->data.at(key).contains(rest...);
	}
};

typedef std::tuple<int, int, int> ZYX;
typedef std::tuple<int, int, int> XYZ;

class Solver
{
	std::filesystem::path filepath;

	std::vector<std::pair<ZYX, ZYX>> get_bricks()
	{
		std::ifstream file(this->filepath);
		std::vector<std::pair<ZYX, ZYX>> bricks;
		for (char c = file.get(); c != EOF; c = file.get())
		{
			// Get start coordinates
			int start[3]{}, *current_num = &start[0];
			for (; c != '~'; c = file.get())
			{
				if (c == ',')
				{
					++current_num;
					continue;
				}

				*current_num = *current_num * 10 + c - '0';
			}

			// Get end coordinates
			int end[3]{};
			current_num = &end[0];
			for (char c = file.get(); c != '\n' && c != EOF; c = file.get())
			{
				if (c == ',')
				{
					++current_num;
					continue;
				}

				*current_num = *current_num * 10 + c - '0';
			}

			// Get length
			ZYX length({end[2] - start[2] + 1,
									end[1] - start[1] + 1,
									end[0] - start[0] + 1}),
					start_coord({start[2], start[1], start[0]});

			bricks.emplace_back(start_coord, length);
		}

		return bricks;
	}

	std::pair<std::vector<std::pair<XYZ, XYZ>>,
						nested_unordered_map<XYZ, int>>
	get_resting_bricks()
	{
		std::vector<std::pair<ZYX, ZYX>> bricks = this->get_bricks();

		auto heap_comp = std::greater<std::pair<ZYX, ZYX>>();
		std::make_heap(bricks.begin(), bricks.end(), heap_comp);
		nested_unordered_map<std::tuple<int, int>, int> lowest_non_blocking_z;
		std::vector<std::pair<XYZ, XYZ>> resting_bricks;
		nested_unordered_map<XYZ, int> blocks_to_id;
		while (!bricks.empty())
		{
			auto [coordinate, lengths] = bricks.front();
			std::pop_heap(bricks.begin(), bricks.end(), heap_comp);
			bricks.pop_back();
			auto [z, y, x] = coordinate;
			auto [l_z, l_y, l_x] = lengths;

			// Find the z position the brick will rest at
			int max_blocking_z = 0;
			for (int d_x = 0; d_x < l_x; ++d_x)
			{
				for (int d_y = 0; d_y < l_y; ++d_y)
				{
					max_blocking_z = std::max(max_blocking_z,
																		1 + lowest_non_blocking_z[{x + d_x, y + d_y}]);
				}
			}
			int min_z = std::min(max_blocking_z, z);

			// Mark the new highest z positions for each of the brick's xy positions
			for (int d_x = 0; d_x < l_x; ++d_x)
			{
				for (int d_y = 0; d_y < l_y; ++d_y)
				{
					std::tuple<int, int> position(x + d_x, y + d_y);
					lowest_non_blocking_z[position] = std::max(lowest_non_blocking_z[position],
																										 min_z + l_z - 1);
				}
			}

			// Mark the block to belong to the current brick
			for (int d_x = 0; d_x < l_x; ++d_x)
			{
				for (int d_y = 0; d_y < l_y; ++d_y)
				{
					for (int d_z = 0; d_z < l_z; ++d_z)
					{
						blocks_to_id[{x + d_x, y + d_y, min_z + d_z}] = resting_bricks.size();
					}
				}
			}
			resting_bricks.emplace_back(
					std::make_tuple(x, y, min_z),
					std::make_tuple(l_x, l_y, l_z));
		}

		return {resting_bricks, blocks_to_id};
	}

	static std::pair<
			std::vector<std::unordered_set<int>>,
			std::vector<std::unordered_set<int>>>
	create_directional_support_graphs(
			const std::vector<std::pair<XYZ, XYZ>> &resting_bricks,
			const nested_unordered_map<XYZ, int> &blocks_to_id)
	{
		std::vector<std::unordered_set<int>> supports(resting_bricks.size()),
				supported_by(resting_bricks.size());

		for (int brick = 0; brick < resting_bricks.size(); ++brick)
		{
			auto [x, y, z] = resting_bricks[brick].first;
			auto [l_x, l_y, _] = resting_bricks[brick].second;
			for (int d_x = 0; d_x < l_x; ++d_x)
			{
				for (int d_y = 0; d_y < l_y; ++d_y)
				{
					if (!blocks_to_id.contains(x + d_x, y + d_y, z - 1))
						continue;

					int brick_below = blocks_to_id[{x + d_x, y + d_y, z - 1}];
					supported_by[brick].insert(brick_below);
					supports[brick_below].insert(brick);
				}
			}
		}

		return {supports, supported_by};
	}

	int part_1()
	{
		auto [resting_bricks,
					blocks_to_id] = this->get_resting_bricks();

		auto [supports,
					supported_by] =
				this->create_directional_support_graphs(resting_bricks, blocks_to_id);

		int result = 0;
		for (const std::unordered_set<int> &supported_bricks : supports)
		{
			bool can_remove = true;
			for (int supported_brick : supported_bricks)
			{
				if (supported_by[supported_brick].size() <= 1)
				{
					can_remove = false;
					break;
				}
			}
			result += can_remove;
		}
		return result;
	}

	int part_2()
	{
		auto [resting_bricks,
					blocks_to_id] = this->get_resting_bricks();

		auto [supports,
					supported_by] =
				this->create_directional_support_graphs(resting_bricks, blocks_to_id);

		int result = 0;
		for (const std::unordered_set<int> &supported_bricks : supports)
		{
			std::stack<int> stack;
			std::unordered_set<int> falling_bricks;
			for (int brick : supported_bricks)
			{
				if (supported_by[brick].size() == 1)
				{
					stack.push(brick);
					falling_bricks.insert(brick);
				}
			}

			while (!stack.empty())
			{
				int brick = stack.top();
				stack.pop();
				for (int candidate_brick : supports[brick])
				{
					if (falling_bricks.contains(candidate_brick))
						continue;

					bool is_valid = true;
					for (int support_brick : supported_by[candidate_brick])
					{
						if (!falling_bricks.contains(support_brick))
						{
							is_valid = false;
							break;
						}
					}
					if (!is_valid)
						continue;

					falling_bricks.insert(candidate_brick);
					stack.push(candidate_brick);
				}
			}

			result += falling_bricks.size();
		}

		return result;
	}

public:
	Solver(std::filesystem::path filepath = "input.txt") : filepath(filepath) {}

	void solve()
	{
		std::cout << "Day 22" << std::endl;
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