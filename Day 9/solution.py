"""
Day 9 solution.
"""
import itertools
import sys


class Solver:
    """Day 9 solver."""

    def __init__(self, filepath: str = "input.txt"):
        self.filepath = filepath

    @staticmethod
    def get_num(line: str, find_last: bool) -> int:
        """
        Get the next number in the sequence.
        """
        nums = [int(x) for x in line.split()]
        sequences = []
        while any(x != y for x, y in itertools.pairwise(nums)):
            sequences.append(nums)
            nums = [y - x for x, y in itertools.pairwise(nums)]

        def get_previous_sequence_nums(num: int) -> int:
            if find_last:
                return num + sequences.pop()[-1]
            return sequences.pop()[0] - num

        num = nums[-1 if find_last else 0]
        while sequences:
            num = get_previous_sequence_nums(num)
        return num

    def generic_solve(self, find_last: bool) -> int:
        """Generic solve."""
        with open(
            self.filepath, "r", encoding=sys.getdefaultencoding()
        ) as file:
            return sum(self.get_num(line, find_last) for line in file)

    def part_1(self) -> int:
        """Part 1 solver."""
        return self.generic_solve(True)

    def part_2(self) -> int:
        """Part 2 solver."""
        return self.generic_solve(False)

    def solve(self) -> None:
        """Runs part 1 and part 2."""
        print("Day 9")
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
