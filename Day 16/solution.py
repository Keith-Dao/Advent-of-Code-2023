"""
Day 16 solution.
"""
import collections
import enum
import sys
from multiprocessing import Pool
from typing import Generator


class Solver:
    """Day 16 solver."""

    class symbols(enum.Enum):
        """Valid input symbols."""

        SPACE = "."
        POSITIVE_MIRROR = "/"
        NEGATIVE_MIRROR = "\\"
        HORIZONTAL_SPLITTER = "-"
        VERTICAL_SPLITTER = "|"

    def __init__(self, filepath: str = "input.txt"):
        self.filepath = filepath

    def get_grid(self) -> list[str]:
        """Parse the grid."""
        with open(
            self.filepath, "r", encoding=sys.getdefaultencoding()
        ) as file:
            return [line.strip() for line in file]

    @staticmethod
    def multiprocessing_get_num_energized_positions_wrapper(
        args: tuple[list[str], int, int, int, int]
    ) -> int:
        """Wrapper to unpack arguments for get_num_energized_positions."""
        return Solver.get_num_energized_positions(*args)

    @staticmethod
    def get_num_energized_positions(
        grid: list[str], i: int, j: int, d_i: int, d_j: int
    ) -> int:
        """Get the number of energized positions."""

        def add_next(
            stack: list[tuple[int, int, int, int]],
            i: int,
            j: int,
            d_i: int,
            d_j: int,
        ):
            c = grid[i][j]
            if c == Solver.symbols.SPACE.value:
                stack.append((i + d_i, j + d_j, d_i, d_j))
            elif c == Solver.symbols.POSITIVE_MIRROR.value:
                stack.append((i - d_j, j - d_i, -d_j, -d_i))
            elif c == Solver.symbols.NEGATIVE_MIRROR.value:
                stack.append((i + d_j, j + d_i, d_j, d_i))
            elif c == Solver.symbols.VERTICAL_SPLITTER.value:
                if d_i != 0:
                    stack.append((i + d_i, j + d_j, d_i, d_j))
                    return

                stack.append((i + 1, j, 1, 0))
                stack.append((i - 1, j, -1, 0))
            elif c == Solver.symbols.HORIZONTAL_SPLITTER.value:
                if d_j != 0:
                    stack.append((i + d_i, j + d_j, d_i, d_j))
                    return

                stack.append((i, j + 1, 0, 1))
                stack.append((i, j - 1, 0, -1))

        energized = collections.defaultdict(set)
        stack = [(i, j, d_i, d_j)]
        while stack:
            i, j, d_i, d_j = stack.pop()

            if not 0 <= i < len(grid) or not 0 <= j < len(grid[0]):
                continue

            if (d_i, d_j) in energized[i, j]:
                continue

            energized[i, j].add((d_i, d_j))
            add_next(stack, i, j, d_i, d_j)

        return len(energized)

    def part_1(self) -> int:
        """Part 1 solver."""
        grid = self.get_grid()
        return self.get_num_energized_positions(grid, 0, 0, 0, 1)

    def part_2(self) -> int:
        """Part 2 solver."""
        grid = self.get_grid()

        def get_start_positions() -> (
            Generator[tuple[list[str], int, int, int, int], None, None]
        ):
            # Start from the left column and go right
            for i in range(len(grid)):
                yield grid, i, 0, 0, 1

            # Start from the right column and go left
            for i, row in enumerate(grid):
                yield grid, i, len(row) - 1, 0, -1

            # Start from the top row and go down
            for j in range(len(grid[0])):
                yield grid, 0, j, 1, 0

            # Start from the bottom row and go up
            for j in range(len(grid[-1])):
                yield grid, len(grid) - 1, j, -1, 0

        with Pool() as pool:
            return max(
                pool.imap(
                    self.multiprocessing_get_num_energized_positions_wrapper,
                    get_start_positions(),
                )
            )

    def solve(self) -> None:
        """Runs part 1 and part 2."""
        print("Day 16")
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
