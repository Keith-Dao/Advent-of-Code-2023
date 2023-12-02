#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <cassert>

struct FileParserIterator
{
	using iterator_category = std::input_iterator_tag;
	using value_type = std::pair<int, std::vector<std::string>>;
	using reference = value_type const &;
	using pointer = value_type const *;
	using difference_type = ptrdiff_t;

	FileParserIterator(std::filesystem::path filepath, bool done = false) : done(done)
	{
		this->file = new std::ifstream();
		this->file->open(filepath);
		done || ++*this;
	}

	FileParserIterator(const FileParserIterator &other) : done(other.done),
																												value(other.value), file(other.file) {}

	~FileParserIterator()
	{
		this->file->close();
		delete this->file;
	}

	explicit operator bool() const { return !done; }

	reference operator*() const { return value; }
	pointer operator->() const { return &value; }

	FileParserIterator &operator++()
	{
		assert(!this->done);

		this->done = this->file->eof();
		if (this->done)
			return *this;

		int game_id = 0;
		std::vector<std::string> sets;

		// Read a line
		char c = 0;
		while (!this->file->eof() && c != '\n')
		{
			while ((c = this->file->get()) != ':')
			{
				if (!std::isdigit(c))
					continue;
				game_id = game_id * 10 + (c - '0');
			}

			while (!this->file->eof() && c != '\n')
			{
				std::string current;
				while ((c = this->file->get()) != ';' && c != '\n' && c != EOF)
				{
					current.push_back(c);
				}
				sets.push_back(current);
				current.clear();
			}
		}

		this->value = std::make_pair(game_id, sets);
		return *this;
	}

	FileParserIterator operator++(int)
	{
		FileParserIterator const tmp(*this);
		++*this;
		return tmp;
	}

	friend bool operator!=(const FileParserIterator &a, const FileParserIterator &b)
	{
		return a.done != b.done;
	};

private:
	bool done;
	value_type value;
	std::ifstream *file = nullptr;
};

class FileParser
{
	std::filesystem::path filepath;
	using Iterator = FileParserIterator;

public:
	FileParser(std::filesystem::path filepath) : filepath(filepath) {}

	Iterator begin() const
	{
		return Iterator(this->filepath);
	}

	Iterator end() const
	{
		return Iterator(this->filepath, true);
	}
};

class Solver
{
	std::filesystem::path filepath;

	std::vector<std::pair<int, int>> decode_set(const std::string &game_set)
	{
		std::vector<std::pair<int, int>> result;
		for (int i = 0; i < game_set.size(); ++i)
		{
			int num = 0;
			std::string colour;

			for (; i < game_set.size() && game_set[i] != ','; ++i)
			{
				char c = game_set[i];
				if (std::isdigit(c))
				{
					num = num * 10 + (c - '0');
				}
				else if (c != ' ')
				{
					colour += c;
				}
			}

			result.emplace_back(num, colour == "red" ? 0 : (colour == "green" ? 1 : 2));
		}
		return result;
	}

	int part_1()
	{
		FileParser file(this->filepath);
		int result = 0, max_colours[3]{12, 13, 14};

		for (auto [game_id, sets] : file)
		{
			bool valid = true;
			for (const std::string &set : sets)
			{
				if (!valid)
					break;
				for (const auto [num, colour] : this->decode_set(set))
				{
					if (num > max_colours[colour])
					{
						valid = false;
						break;
					}
				}
			}
			if (valid)
			{
				result += game_id;
			}
		}
		return result;
	}

	int part_2()
	{
		FileParser file(this->filepath);
		int result = 0;

		for (auto [_, sets] : file)
		{
			int colours[3]{0, 0, 0};
			for (const std::string &set : sets)
			{
				for (const auto [num, colour] : this->decode_set(set))
				{
					colours[colour] = std::max(colours[colour], num);
				}
			}

			result += colours[0] * colours[1] * colours[2];
		}
		return result;
	}

public:
	Solver(std::filesystem::path filepath = "input.txt") : filepath(filepath) {}

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