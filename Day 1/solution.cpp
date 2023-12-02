#include <filesystem>
#include <fstream>
#include <iostream>

struct TrieNode
{
	TrieNode *children[26]{nullptr};
	int val = -1;

	~TrieNode()
	{
		for (int i = 0; i < 26; ++i)
		{
			if (this->children[i] != nullptr)
			{
				delete this->children[i];
			}
		}
	}

	void set(char c)
	{
		int i = c - 'a';
		if (this->children[i] == nullptr)
		{
			this->children[i] = new TrieNode();
		}
	}

	bool contains(char c)
	{
		return this->children[c - 'a'] != nullptr;
	}

	TrieNode *get(char c)
	{
		this->set(c);
		return this->children[c - 'a'];
	}
};

class Solver
{
	std::filesystem::path filepath;
	TrieNode *root;

	int substr_to_num(std::string substr)
	{
		TrieNode *curr = this->root;
		for (int i = substr.size() - 1; i >= 0; --i)
		{
			if (!curr->contains(substr[i]))
			{
				break;
			}
			curr = curr->get(substr[i]);

			if (curr->val != -1)
			{
				return curr->val;
			}
		}
		return -1;
	}

	int general_solver(bool include_word_numbers = false)
	{
		std::ifstream file;
		file.open(this->filepath);
		int result = 0, first = -1, last = -1;
		std::string substr = "";

		while (!file.eof())
		{
			char c = file.get();
			if (c == '\n' || c == EOF)
			{
				result += first * 10 + last;
				first = last = -1;
				substr.clear();
				continue;
			}

			int num;
			if (std::isdigit(c))
			{
				num = c - '0';
				substr.clear();
			}
			else if (!include_word_numbers)
			{
				continue;
			}
			else
			{
				substr.push_back(c);
				num = this->substr_to_num(substr);
				if (num == -1)
					continue;
			}

			if (first == -1)
				first = num;
			last = num;
		}

		file.close();
		return result;
	}

	int part_1()
	{
		return this->general_solver(false);
	}

	int part_2()
	{
		return this->general_solver(true);
	}

public:
	Solver(std::filesystem::path filepath = "input.txt") : filepath(filepath)
	{
		this->root = new TrieNode();
		std::string words[9]{
				"one", "two", "three", "four", "five", "six", "seven", "eight", "nine"};
		for (int num = 0; num < 9; ++num)
		{
			TrieNode *curr = this->root;
			for (int i = words[num].size() - 1; i >= 0; --i)
			{
				curr = curr->get(words[num][i]);
			}
			curr->val = num + 1;
		}
	};

	~Solver()
	{
		delete this->root;
	}

	void solve()
	{
		std::cout << "Day 1" << std::endl;
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