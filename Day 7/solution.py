"""
Day 7 solution.
"""
from __future__ import annotations

import collections
import enum
import sys


class Hand:
    """
    A hand with the cards and the associated bet.
    """

    class types(enum.Enum):
        """Possible hand types."""

        HIGH_CARD = 0
        ONE_PAIR = 1
        TWO_PAIR = 2
        THREE_OF_A_KIND = 3
        FULL_HOUSE = 4
        FOUR_OF_A_KIND = 5
        FIVE_OF_A_KIND = 6

    def _get_hand_type(self, include_wildcard: bool) -> types:
        """Gets the hand type."""
        deck = collections.Counter(self.hand)
        if len(deck) == 1:
            return self.types.FIVE_OF_A_KIND

        wildcards = 0
        if include_wildcard and "J" in deck:
            wildcards = deck.pop("J")
        ordered_count = sorted(deck.values(), reverse=True)
        ordered_count[0] += wildcards

        if ordered_count[0] == 5:
            return self.types.FIVE_OF_A_KIND

        if ordered_count[0] == 4:
            return self.types.FOUR_OF_A_KIND

        if ordered_count[0] == 3:
            return (
                self.types.FULL_HOUSE
                if ordered_count[1] == 2
                else self.types.THREE_OF_A_KIND
            )

        if ordered_count[0] == 2:
            return (
                self.types.TWO_PAIR
                if ordered_count[1] == 2
                else self.types.ONE_PAIR
            )

        return self.types.HIGH_CARD

    def __init__(self, line: str, include_wildcard: bool = False) -> None:
        self.hand, bet = line.split()
        self.bet = int(bet)
        self.type = self._get_hand_type(include_wildcard)

        self.type_to_strength = {
            c: i for i, c in enumerate(reversed("AKQJT98765432"))
        }
        if include_wildcard:
            self.type_to_strength["J"] = -1

    def __lt__(self, other: Hand) -> bool:
        if self.type != other.type:
            return self.type.value < other.type.value

        for a, b in zip(self.hand, other.hand):
            if a != b:
                return self.type_to_strength[a] < other.type_to_strength[b]
        return False


class Solver:
    """Day 7 solver."""

    def __init__(self, filepath: str = "input.txt"):
        self.filepath = filepath

    def generic_solve(self, include_wildcard: bool = False) -> int:
        """Generic solve."""
        with open(
            self.filepath, "r", encoding=sys.getdefaultencoding()
        ) as file:
            hands = [Hand(line, include_wildcard) for line in file]

        return sum(
            i * hand.bet for i, hand in enumerate(sorted(hands), start=1)
        )

    def part_1(self) -> int:
        """Part 1 solver."""
        return self.generic_solve()

    def part_2(self) -> int:
        """Part 2 solver."""
        return self.generic_solve(True)

    def solve(self) -> None:
        """Runs part 1 and part 2."""
        print("Day 7")
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
        case _:
            print("Usage: python solution.py <path>")
            sys.exit(0)
    solver.solve()
