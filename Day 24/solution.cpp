#include <filesystem>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <unordered_set>
#include <array>
#include <cassert>

typedef std::array<long, 3> coords;

class Solver
{
	std::filesystem::path filepath;

	std::vector<std::pair<coords, coords>> get_hailstones()
	{
		std::ifstream file(this->filepath);
		std::vector<std::pair<coords, coords>> result;
		for (char c = file.get(); c != EOF; c = file.get())
		{
			long position[3]{}, *curr = &position[0];
			for (; c != '@'; c = file.get())
			{
				switch (c)
				{
				case ',':
					++curr;
					break;
				case ' ':
					break;
				default:
					*curr = *curr * 10 + c - '0';
				}
			}

			bool negative = false;
			long velocity[3]{};
			curr = &velocity[0];
			for (c = file.get(); c != '\n' && c != EOF; c = file.get())
			{
				switch (c)
				{
				case '-':
					negative = true;
					break;
				case ',':
					if (negative)
					{
						*curr *= -1;
						negative = false;
					}
					++curr;
					break;
				case ' ':
					break;
				default:
					*curr = *curr * 10 + c - '0';
				}
			}
			if (negative)
			{
				*curr *= -1;
			}

			result.emplace_back(
					coords{{position[0], position[1], position[2]}},
					coords{{velocity[0], velocity[1], velocity[2]}});
		}

		return result;
	}

	int part_1()
	{
		std::vector<std::pair<coords, coords>> hailstones = this->get_hailstones();
		const long LOWER_BOUND = 200000000000000, UPPER_BOUND = 400000000000000;

		int result = 0;
		for (int i = 0; i < hailstones.size(); ++i)
		{
			for (int j = 0; j < i; ++j)
			{
				auto [p_0, v_0] = hailstones[i];
				auto [p_1, v_1] = hailstones[j];

				long x_0 = p_0[0], y_0 = p_0[1], vx_0 = v_0[0], vy_0 = v_0[1],
						 x_1 = p_1[0], y_1 = p_1[1], vx_1 = v_1[0], vy_1 = v_1[1];

				if (vy_1 * vx_0 - vy_0 * vx_1 == 0)
					continue;

				double divisor = vy_1 * vx_0 - vy_0 * vx_1;
				double x = vx_1 / divisor * vx_0 * (y_0 - y_1) + vy_1 / divisor * vx_0 * x_1 - vy_0 / divisor * vx_1 * x_0;
				if ((x - x_0) / vx_0 <= 0 || (x - x_1) / vx_1 <= 0)
					continue;

				double y = y_0 + vy_0 * (x - x_0) / vx_0;

				result += LOWER_BOUND <= x && x <= UPPER_BOUND && LOWER_BOUND <= y && y <= UPPER_BOUND;
			}
		}

		return result;
	}

	long part_2()
	{
		std::vector<std::pair<coords, coords>> hailstones = this->get_hailstones();

		std::unordered_set<int> potential_axes_velocities[3];
		for (int i = 0; i < hailstones.size(); ++i)
		{
			auto [positions_0, velocities_0] = hailstones[i];
			for (int j = 0; j < i; ++j)
			{
				auto [positions_1, velocities_1] = hailstones[j];
				for (int k = 0; k < 3; ++k)
				{
					if (velocities_0[k] != velocities_1[k])
						continue;

					long difference = positions_1[k] - positions_0[k];
					std::unordered_set<int> candidate_velocities;
					for (int v = -1000; v < 1000; ++v)
					{
						if (v == velocities_0[k])
							continue;
						if (difference % (v - velocities_0[k]) == 0)
							candidate_velocities.insert(v);
					}

					if (potential_axes_velocities[k].empty())
					{
						potential_axes_velocities[k] = candidate_velocities;
					}
					else
					{
						std::unordered_set<int> temp;
						for (int x : potential_axes_velocities[k])
						{
							if (candidate_velocities.contains(x))
								temp.insert(x);
						}
						potential_axes_velocities[k] = temp;
					}
				}
			}
		}

		for (int i = 0; i < 3; ++i)
			assert(potential_axes_velocities[i].size() == 1);

		long vx = *potential_axes_velocities[0].begin(),
				 vy = *potential_axes_velocities[1].begin(),
				 vz = *potential_axes_velocities[2].begin();
		auto [p_0, v_0] = hailstones[0];
		auto [p_1, v_1] = hailstones[1];
		long x_0 = p_0[0], y_0 = p_0[1], z_0 = p_0[2],
				 vx_0 = v_0[0], vy_0 = v_0[1], vz_0 = v_0[2],
				 x_1 = p_1[0], y_1 = p_1[1],
				 vx_1 = v_1[0], vy_1 = v_1[1];

		double m_0 = (vy_0 - vy) / (double)(vx_0 - vx),
					 m_1 = (vy_1 - vy) / (double)(vx_1 - vx),
					 c_0 = y_0 - (m_0 * x_0),
					 c_1 = y_1 - (m_1 * x_1);

		long x = (c_1 - c_0) / (m_0 - m_1),
				 y = (m_0 * x + c_0),
				 t = (x - x_0) / (vx_0 - vx),
				 z = z_0 + (vz_0 - vz) * t;

		return x + y + z;
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