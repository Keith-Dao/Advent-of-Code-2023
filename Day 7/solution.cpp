#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>

enum types
{
	HIGH_CARD = 0,
	ONE_PAIR = 1,
	TWO_PAIR = 2,
	THREE_OF_A_KIND = 3,
	FULL_HOUSE = 4,
	FOUR_OF_A_KIND = 5,
	FIVE_OF_A_KIND = 6,
};

struct Hand
{
	std::string hand;
	int bet = 0;
	types type;
	std::unordered_map<char, int> type_to_strength;

	Hand(std::ifstream &file, bool include_wildcard = false)
	{
		for (int i = 0; i < 5; ++i)
			this->hand += file.get();
		while (file.peek() == ' ')
			file.get();
		while (file.peek() != '\n' && file.peek() != EOF)
			this->bet = this->bet * 10 + file.get() - '0';
		file.get(); // Go to new line or EOF

		this->type = this->get_hand_type(include_wildcard);

		std::string card_order = "AKQJT98765432";
		for (int i = 0; i < card_order.size(); ++i)
		{
			this->type_to_strength[card_order.end()[~i]] = i;
		}
		if (include_wildcard)
		{
			this->type_to_strength['J'] = -1;
		}
	}

	bool operator<(const Hand &other) const
	{
		if (this->type != other.type)
		{
			return this->type < other.type;
		}

		for (int i = 0; i < this->hand.size(); ++i)
		{
			char a = this->hand[i], b = other.hand[i];
			if (a != b)
			{
				return (this->type_to_strength.at(a)) < (other.type_to_strength.at(b));
			}
		}
		return false;
	}

private:
	types get_hand_type(bool include_wildcard)
	{
		std::unordered_map<char, int> deck;
		for (char c : this->hand)
			++deck[c];
		if (deck.size() == 1)
			return FIVE_OF_A_KIND;

		int wildcards = 0;
		if (include_wildcard && deck.contains('J'))
		{
			wildcards = deck['J'];
			deck.erase('J');
		}

		std::vector<int> ordered_count;
		for (auto [_, x] : deck)
		{
			ordered_count.push_back(x);
		}
		std::sort(ordered_count.begin(), ordered_count.end(), std::greater<int>());
		ordered_count.front() += wildcards;

		switch (ordered_count.front())
		{
		case 5:
			return FIVE_OF_A_KIND;
		case 4:
			return FOUR_OF_A_KIND;
		case 3:
			return ordered_count[1] == 2 ? FULL_HOUSE : THREE_OF_A_KIND;
		case 2:
			return ordered_count[1] == 2 ? TWO_PAIR : ONE_PAIR;
		default:
			return HIGH_CARD;
		}
	}
};

class Solver
{
	std::filesystem::path filepath;

	int generic_solve(bool include_wildcard = false)
	{
		std::ifstream file;
		file.open(this->filepath);
		std::vector<Hand> hands;
		while (!file.eof())
		{
			hands.emplace_back(file, include_wildcard);
		}
		file.close();

		std::sort(hands.begin(), hands.end());
		int result = 0;
		for (int i = 0; i < hands.size(); ++i)
		{
			result += (i + 1) * hands[i].bet;
		}

		return result;
	}

	int part_1()
	{
		return this->generic_solve();
	}

	int part_2()
	{
		return this->generic_solve(true);
	}

public:
	Solver(std::filesystem::path filepath = "input.txt") : filepath(filepath) {}

	void solve()
	{
		std::cout << "Day 7" << std::endl;
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