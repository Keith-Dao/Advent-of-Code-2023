#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <cassert>
#include <unordered_map>

struct FileParserIterator
{
	using iterator_category = std::input_iterator_tag;
	using value_type = std::string;
	using reference = value_type const &;
	using pointer = value_type const *;
	using difference_type = ptrdiff_t;

	FileParserIterator(std::filesystem::path filepath, bool done = false) : done(done)
	{
		this->file = new std::ifstream();
		this->file->open(filepath);
		if (!done)
			this->get_next();
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

	void get_next()
	{
		this->value.clear();
		if (this->file->eof())
		{
			this->done = true;
			return;
		}

		for (char c = this->file->get(); c != ',' && c != EOF; c = this->file->get())
		{
			this->value += c;
		}
	}

	FileParserIterator &operator++()
	{
		assert(!this->done);
		this->get_next();
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

template <typename K, typename V>
class OrderedMap
{
	struct DoublyLinkedList
	{
		K key;
		V value;
		DoublyLinkedList *prev, *next;

		DoublyLinkedList(
				K key, V value,
				DoublyLinkedList *prev = nullptr,
				DoublyLinkedList *next = nullptr) : key(key), value(value), prev(prev), next(next) {}
	};

	struct ListNodeIterator
	{
		using iterator_category = std::input_iterator_tag;
		using value_type = V;
		using reference = value_type const &;
		using pointer = value_type const *;
		using difference_type = ptrdiff_t;

		ListNodeIterator(DoublyLinkedList *node) : node(node)
		{
			if (node != nullptr)
			{
				this->value = node->value;
			}
		}

		ListNodeIterator(const ListNodeIterator &other) : node(other.node),
																											value(other.value) {}

		explicit operator bool() const { return this->node != nullptr; }

		reference operator*() const { return value; }
		pointer operator->() const { return &value; }

		ListNodeIterator &operator++()
		{
			assert(this->node != nullptr);
			this->node = node->next;
			if (this->node != nullptr)
				this->value = this->node->value;
			return *this;
		}

		ListNodeIterator operator++(int)
		{
			ListNodeIterator const tmp(*this);
			++*this;
			return tmp;
		}

		friend bool operator!=(const ListNodeIterator &a, const ListNodeIterator &b)
		{
			return a.node != b.node;
		};

	private:
		value_type value;
		DoublyLinkedList *node;
	};

	std::unordered_map<K, DoublyLinkedList *> map;
	DoublyLinkedList *head, *tail;
	using Iterator = ListNodeIterator;

public:
	OrderedMap()
	{
		this->head = new DoublyLinkedList({}, {});
		this->tail = new DoublyLinkedList({}, {}, this->head);
		this->head->next = this->tail;
	}

	~OrderedMap()
	{
		DoublyLinkedList *curr = this->head, *next;
		while (curr != nullptr)
		{
			next = curr->next;
			delete curr;
			curr = next;
		}
	}

	bool contains(const K &key) const
	{
		return this->map.contains(key);
	}

	void remove(const K &key)
	{
		if (!this->contains(key))
			return;

		DoublyLinkedList *node = this->map[key];
		this->map.erase(key);
		node->prev->next = node->next;
		node->next->prev = node->prev;
		delete node;
	}

	void add(const K &key, const V &value)
	{
		if (!this->contains(key))
		{
			DoublyLinkedList *node = new DoublyLinkedList(key, value, this->tail->prev, this->tail);
			node->prev->next = this->tail->prev = node;
			this->map[key] = node;
		}
		this->map[key]->value = value;
	}

	V operator[](const K &key) const
	{
		return this->map.at(key).value;
	}

	Iterator begin()
	{
		return Iterator(this->head->next);
	}

	Iterator end()
	{
		return Iterator(this->tail);
	}
};

typedef std::vector<std::vector<int>> row_list;

class Solver
{
	std::filesystem::path filepath;

	int get_hash_code(const std::string &tokens)
	{
		int total = 0;
		for (char c : tokens)
		{
			total += c;
			total *= 17;
			total %= 256;
		}
		return total;
	}

	int part_1()
	{
		FileParser file(this->filepath);
		int result = 0;
		for (const std::string &sequence : file)
		{
			result += this->get_hash_code(sequence);
		}
		return result;
	}

	int part_2()
	{
		char REMOVE = '-', REPLACE = '=';
		int num_boxes = 256;
		OrderedMap<std::string, int> boxes[num_boxes];

		FileParser file(this->filepath);
		for (const std::string &sequence : file)
		{
			int focal_length = 0;
			std::string label;
			char op = 0;

			for (char c : sequence)
			{
				if (op != 0)
				{
					focal_length = focal_length * 10 + c - '0';
				}
				else if (c == REMOVE || c == REPLACE)
				{
					op = c;
				}
				else
				{
					label += c;
				}
			}
			int hash_code = this->get_hash_code(label);

			if (op == REMOVE)
			{
				boxes[hash_code].remove(label);
			}
			else if (op == REPLACE)
			{
				boxes[hash_code].add(label, focal_length);
			}
		}

		int result = 0;
		for (int slot_num = 1; slot_num <= num_boxes; ++slot_num)
		{
			int box_num = 1;
			for (int focal_length : boxes[slot_num - 1])
			{
				result += box_num++ * slot_num * focal_length;
			}
		}
		return result;
	}

public:
	Solver(std::filesystem::path filepath = "input.txt") : filepath(filepath) {}

	void solve()
	{
		std::cout << "Day 15" << std::endl;
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