"""
Day 12 solution.
"""
import sys


class Solver:
    """Day 12 solver."""

    def __init__(self, filepath: str = "input.txt"):
        self.filepath = filepath

    @staticmethod
    def count_arrangements(
        symbols: str, nums: list[int], dp: list[list[int]], i: int, j: int
    ) -> int:
        """Counts the number of possible arrangements."""

        def count_arrangements(
            symbols: str, nums: list[int], dp: list[list[int]], i: int, j: int
        ) -> int:
            result = 0

            while i < len(symbols) and symbols[i] == ".":  # Skip leading .
                i += 1

            if i >= len(symbols):
                return result + (j == len(nums))

            if j == len(nums):
                while i < len(symbols) and symbols[i] in [".", "?"]:
                    i += 1
                return result + (i == len(symbols))

            if symbols[i] == "?":  # Replace "?" with "."
                result += Solver.count_arrangements(
                    symbols, nums, dp, i + 1, j
                )

            count = 0
            while i < len(symbols) and symbols[i] != "." and count < nums[j]:
                i += 1
                count += 1
            if count < nums[j]:  # Not enough # or ? for the current count
                return result

            if (
                i < len(symbols) and symbols[i] == "#"
            ):  # Too many # for the current count
                return result

            return result + Solver.count_arrangements(
                symbols, nums, dp, i + 1, j + 1
            )

        if dp[i][j] == -1:
            dp[i][j] = count_arrangements(symbols, nums, dp, i, j)
        return dp[i][j]

    @staticmethod
    def parse_line(
        line: str, num_repeat: int = 1
    ) -> tuple[str, list[int], list[list[int]]]:
        """
        Parse the line to get the symbol and number data then generate the repeated
        symbols, repeated numbers and the dp grid.
        """
        symbols, num_data = line.split()
        symbols = "?".join(symbols for _ in range(num_repeat))
        nums = [int(x) for x in num_data.split(",")] * num_repeat
        dp = [[-1] * (len(nums) + 1) for _ in range(len(symbols) + 2)]
        return symbols, nums, dp

    def generic_solve(self, num_repeat: int = 1) -> int:
        """Generic solve"""
        with open(
            self.filepath, "r", encoding=sys.getdefaultencoding()
        ) as file:
            return sum(
                self.count_arrangements(
                    *self.parse_line(line, num_repeat), 0, 0
                )
                for line in file
            )

    def part_1(self) -> int:
        """Part 1 solver."""
        return self.generic_solve(1)

    def part_2(self) -> int:
        """Part 2 solver."""
        return self.generic_solve(5)

    def solve(self) -> None:
        """Runs part 1 and part 2."""
        print("Day 12")
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
