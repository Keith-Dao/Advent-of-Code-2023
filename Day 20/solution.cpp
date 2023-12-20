#include <filesystem>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <functional>
#include <unordered_map>
#include <queue>
#include <unordered_set>
#include <numeric>

enum module_types
{
	BROADCASTER = 'b',
	FLIP_FLOP = '%',
	CONJUNCTION = '&'
};

enum pulse_type
{
	LOW_PULSE = false,
	HIGH_PULSE = true
};

class Solver
{
	std::filesystem::path filepath;

	std::tuple<
			std::unordered_map<std::string, std::vector<std::string>>,
			std::unordered_map<std::string, char>,
			std::unordered_map<std::string, bool>,
			std::unordered_map<std::string, std::unordered_map<std::string, bool>>>
	parse_file()
	{
		std::unordered_map<std::string, std::vector<std::string>> modules_outputs;
		std::unordered_map<std::string, char> modules_type;
		std::unordered_map<std::string, bool> modules_state;
		std::unordered_map<std::string, std::unordered_map<std::string, bool>> modules_inputs;

		auto add_next_module = [&modules_outputs,
														&modules_inputs](
															 const std::string &module,
															 const std::string &next_module)
		{
			modules_outputs[module].push_back(next_module);
			modules_inputs[next_module][module] = LOW_PULSE;
		};

		std::ifstream file(this->filepath);
		for (char c = file.get(); c != EOF; c = file.get())
		{
			char module_type = c;
			if (c != BROADCASTER && c != FLIP_FLOP && c != CONJUNCTION)
				throw "Invalid module type.";

			std::string module;
			if (c == BROADCASTER)
				module += c;
			for (c = file.get(); c != ' '; c = file.get())
			{
				module += c;
			}
			modules_type[module] = module_type;
			modules_state[module] = LOW_PULSE;

			// Skip " -> "
			for (c = file.get(); c != ' '; c = file.get())
			{
			}

			std::string next_module;
			for (c = file.get(); c != '\n' && c != EOF; c = file.get())
			{
				switch (c)
				{
				case ',':
					add_next_module(module, next_module);
					next_module.clear();
				case ' ':
					continue;
				default:
					next_module += c;
				}
			}
			add_next_module(module, next_module);
		}

		return std::make_tuple(modules_outputs, modules_type, modules_state, modules_inputs);
	}

	static bool send_pulse(
			const std::unordered_map<std::string, char> &modules_type,
			std::unordered_map<std::string, bool> &modules_state,
			std::unordered_map<std::string, std::unordered_map<std::string, bool>> &modules_inputs,
			const std::string &module,
			bool pulse,
			const std::string prev)
	{
		char module_type = modules_type.at(module);
		if (module_type == BROADCASTER)
			return true;

		if (module_type != FLIP_FLOP && module_type != CONJUNCTION)
			return false;

		if (module_type == FLIP_FLOP)
		{
			if (pulse)
				return false;

			modules_state[module] ^= true;
			return true;
		}

		modules_inputs[module][prev] = pulse;
		bool module_state = false;
		for (auto [_, x] : modules_inputs[module])
		{
			if (!x)
			{
				module_state = true;
				break;
			}
		}
		modules_state[module] = module_state;
		return true;
	}

	long part_1()
	{
		auto [modules_outputs, modules_type, modules_state, modules_inputs] = this->parse_file();

		int pulses[2]{};
		for (int i = 0; i < 1000; ++i)
		{
			std::queue<std::tuple<std::string, bool, std::string>> queue;
			queue.emplace("broadcaster", LOW_PULSE, "");
			++pulses[LOW_PULSE];

			while (!queue.empty())
			{
				auto [module, pulse, prev] = queue.front();
				queue.pop();

				if (!this->send_pulse(modules_type, modules_state, modules_inputs, module, pulse, prev))
				{
					continue;
				}

				bool module_state = modules_state[module];
				for (const std::string &next_module : modules_outputs[module])
				{
					++pulses[module_state];
					if (!modules_type.contains(next_module))
						continue;
					queue.emplace(next_module, module_state, module);
				}
			}
		}
		return pulses[0] * pulses[1];
	}

	long part_2()
	{
		auto [modules_outputs, modules_type, modules_state, modules_inputs] = this->parse_file();

		if (!modules_inputs.contains("rx"))
		{
			std::cout << "rx module missing from input." << std::endl;
			return -1;
		}

		std::unordered_set<std::string> contingent_modules;
		for (auto [prev, _] : modules_inputs["rx"])
		{
			for (auto [module, _] : modules_inputs[prev])
			{
				contingent_modules.insert(module);
			}
		}
		long result = 1;
		std::vector<int> cycles;
		int presses = 0;

		while (!contingent_modules.empty())
		{
			++presses;
			std::queue<std::tuple<std::string, bool, std::string>> queue;
			queue.emplace("broadcaster", LOW_PULSE, "");

			while (!queue.empty())
			{
				auto [module, pulse, prev] = queue.front();
				queue.pop();

				if (contingent_modules.contains(module) && modules_state[module])
				{
					contingent_modules.erase(module);
					result = std::lcm(result, presses);
				}

				if (!this->send_pulse(modules_type, modules_state, modules_inputs, module, pulse, prev))
				{
					continue;
				}

				bool module_state = modules_state[module];
				for (const std::string &next_module : modules_outputs[module])
				{
					if (!modules_type.contains(next_module))
						continue;
					queue.emplace(next_module, module_state, module);
				}
			}
		}

		return result;
	}

public:
	Solver(std::filesystem::path filepath = "input.txt") : filepath(filepath) {}

	void solve()
	{
		std::cout << "Day 20" << std::endl;
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