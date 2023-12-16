"""
Day 1 solution.
"""
from __future__ import annotations

import sys
from collections import defaultdict
from typing import Any


class TrieNode:
    """Trie node."""

    def __init__(self) -> None:
        self.children = defaultdict(TrieNode)
        self.val: int | None = None

    def __contains__(self, key: Any) -> bool:
        return key in self.children

    def __getitem__(self, key: Any) -> TrieNode:
        return self.children[key]


class Solver:
    """Day 1 solver."""

    words = [
        "one",
        "two",
        "three",
        "four",
        "five",
        "six",
        "seven",
        "eight",
        "nine",
    ]

    def __init__(self, filepath: str = "input.txt"):
        self.filepath = filepath

        # Build trie for word to num mapping
        self.trie = TrieNode()
        for num, word in enumerate(self.words, start=1):
            curr = self.trie
            for c in reversed(word):
                curr = curr[c]
            curr.val = num

    def general_solve(self, include_word_numbers: bool = False) -> int:
        """Solving algorithm for part 1 and part 2."""

        def substr_to_num(substr: list[str]) -> int:
            """
            Converts the last substr to a number if valid, else returns -1.
            """
            curr = self.trie
            for c in reversed(substr):
                if c not in curr:
                    break
                curr = curr[c]

                if curr.val is not None:
                    return curr.val
            return -1

        def line_to_num(line: str) -> int:
            first = last = -1
            chars: list[str] = []
            for c in line:
                if c.isdigit():
                    num = int(c)
                    chars.clear()
                elif not include_word_numbers:
                    continue
                else:
                    chars.append(c)
                    num = substr_to_num(chars)
                    if num == -1:
                        continue

                if first == -1:
                    first = num
                last = num
            return first * 10 + last

        with open(
            self.filepath, "r", encoding=sys.getdefaultencoding()
        ) as file:
            return sum(line_to_num(line) for line in file)

    def part_1(self):
        """Part 1 solver."""
        return self.general_solve(False)

    def part_2(self):
        """Part 2 solver."""
        return self.general_solve(True)

    def solve(self) -> None:
        """Runs part 1 and part 2."""
        print("Day 1")
        print(f"Solving: {self.filepath}")
        print("Part 1:")
        print(self.part_1())
        print("---")
        print("Part 2:")
        print(self.part_2())


if __name__ == "__main__":
    match sys.argv:
        case [_]:
            solver = Solver()
        case [_, "--path", path] | [_, "-p", path] | [_, path]:
            solver = Solver(path)
        case other:
            print("Usage: python solution.py <path>")
            sys.exit(0)
    solver.solve()
