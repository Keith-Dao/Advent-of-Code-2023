#include <filesystem>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <cassert>

constexpr auto max_size = std::numeric_limits<std::streamsize>::max();

struct InstructionsIterator
{
	using iterator_category = std::input_iterator_tag;
	using value_type = std::pair<int, std::pair<int, int>>;
	using reference = value_type const &;
	using pointer = value_type const *;
	using difference_type = ptrdiff_t;

	InstructionsIterator(std::filesystem::path filepath, bool part_1, bool done = false) : done(done), part_1(part_1)
	{
		this->file = new std::ifstream();
		this->file->open(filepath);
		if (!done)
			this->get_next();
	}

	InstructionsIterator(const InstructionsIterator &other) : done(other.done),
																														value(other.value), file(other.file) {}

	~InstructionsIterator()
	{
		this->file->close();
		delete this->file;
	}

	explicit operator bool() const { return !done; }

	reference operator*() const { return value; }
	pointer operator->() const { return &value; }

	InstructionsIterator &operator++()
	{
		assert(!this->done);
		this->get_next();
		return *this;
	}

	InstructionsIterator operator++(int)
	{
		InstructionsIterator const tmp(*this);
		++*this;
		return tmp;
	}

	friend bool operator!=(const InstructionsIterator &a, const InstructionsIterator &b)
	{
		return a.done != b.done;
	};

private:
	bool done, part_1;
	value_type value;
	std::ifstream *file = nullptr;

	std::pair<int, int> dir_map(char c)
	{
		switch (c)
		{
		case 'L':
			return {-1, 0};
		case 'R':
			return {1, 0};
		case 'D':
			return {0, -1};
		case 'U':
			return {0, 1};
		default:
			throw "Invalid case";
		}
	}

	std::pair<int, int> num_to_dir(int i)
	{
		char dirs[4]{'R', 'D', 'L', 'U'};
		return this->dir_map(dirs[i]);
	}

	int hex_digit_to_num(char c)
	{
		if ('0' <= c && c <= '9')
			return c - '0';
		return 10 + c - 'a';
	}

	void get_next_part_1()
	{
		if (this->done = this->file->eof())
			return;

		char c = this->file->get();

		// Get direction
		char dir = c;

		// Skip spaces
		for (c = this->file->get(); c == ' '; c = this->file->get())
		{
		}

		int length = 0;
		for (; c != ' '; c = this->file->get())
			length = length * 10 + c - '0';
		this->value = {length, this->dir_map(dir)};

		// Skip to end of line
		this->file->ignore(max_size, '\n');
	}

	void get_next_part_2()
	{
		if (this->done = this->file->eof())
			return;

		// Skip inital tokens
		this->file->ignore(max_size, '#');

		int length = 0;
		for (int i = 0; i < 5; ++i)
			length = length * 16 + this->hex_digit_to_num(this->file->get());
		this->value = {length, this->num_to_dir(this->file->get() - '0')};

		// Skip to end of line
		this->file->ignore(max_size, '\n');
	}

	void get_next()
	{
		if (this->part_1)
			return this->get_next_part_1();
		this->get_next_part_2();
	}
};

class Instructions
{
	std::filesystem::path filepath;
	bool part_1;
	using Iterator = InstructionsIterator;

public:
	Instructions(std::filesystem::path filepath, bool part_1) : filepath(filepath), part_1(part_1) {}

	Iterator begin() const
	{
		return Iterator(this->filepath, this->part_1);
	}

	Iterator end() const
	{
		return Iterator(this->filepath, this->part_1, true);
	}
};

class Solver
{
	std::filesystem::path filepath;

	long generic_solve(const Instructions &instructions)
	{
		long area = 0, edges = 0;
		int x_0 = 0, y_0 = 0;
		for (auto [length, d_x_d_y] : instructions)
		{
			auto [d_x, d_y] = d_x_d_y;
			long x_1 = x_0 + length * d_x, y_1 = y_0 + length * d_y;
			// Shoelace formula
			// https://en.m.wikipedia.org/wiki/Shoelace_formula#Triangle_formula
			area += x_1 * y_0 - y_1 * x_0;
			edges += length;
			x_0 = x_1;
			y_0 = y_1;
		}

		// Apply Pick's theorem to find the inner area then re-add the edge.
		// https://en.wikipedia.org/wiki/Pick%27s_theorem#Formula
		return (std::abs(area) + edges) / 2 + 1;
	}

	long part_1()
	{
		Instructions instructions(this->filepath, true);
		return this->generic_solve(instructions);
	}

	long part_2()
	{
		Instructions instructions(this->filepath, false);
		return this->generic_solve(instructions);
	}

public:
	Solver(std::filesystem::path filepath = "input.txt") : filepath(filepath) {}

	void solve()
	{
		std::cout << "Day 18" << std::endl;
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