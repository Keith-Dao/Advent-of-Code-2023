"""
Day 4 solution.
"""
import sys
from collections import deque


class Solver:
    """Day 4 solver."""

    def __init__(self, filepath: str = "input.txt"):
        self.filepath = filepath

    def parse_line(self, line: str) -> tuple[set[str], list[str]]:
        """
        Parse line for the winning numbers and the received numbers
        """
        game_details = line.split(":")[1]
        winning_nums, received_nums = game_details.split("|")
        return set(winning_nums.split()), received_nums.split()

    def part_1(self):
        """Part 1 solver."""

        def score_line(line: str) -> int:
            result = 0
            winning_numbers, gotten_numbers = self.parse_line(line)
            for num in gotten_numbers:
                if num not in winning_numbers:
                    continue

                if result == 0:
                    result = 1
                else:
                    result *= 2
            return result

        with open(
            self.filepath, "r", encoding=sys.getdefaultencoding()
        ) as file:
            return sum(score_line(line) for line in file)

    def part_2(self):
        """Part 2 solver."""

        def calculate_extra_cards(line: str, extra_cards: deque) -> int:
            winning_numbers, gotten_numbers = self.parse_line(line)
            current_number_cards = (
                0 if not extra_cards else extra_cards.popleft()
            ) + 1

            number_matching = sum(
                1 for num in gotten_numbers if num in winning_numbers
            )

            for i in range(number_matching):
                if len(extra_cards) == i:
                    extra_cards.append(current_number_cards)
                else:
                    extra_cards[i] += current_number_cards

            return number_matching * current_number_cards + 1

        extra_cards = deque([])
        with open(
            self.filepath, "r", encoding=sys.getdefaultencoding()
        ) as file:
            return sum(
                calculate_extra_cards(line, extra_cards) for line in file
            )

    def solve(self) -> None:
        """Runs part 1 and part 2."""
        print("Day 4")
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
