"""
Day 3 solution.
"""
import itertools
import sys
from io import TextIOWrapper
from typing import Callable, Generator, Iterable


class Solver:
    """Day 3 solver."""

    def __init__(self, filepath: str = "input.txt"):
        self.filepath = filepath

    def get_nums_and_symbols(
        self, file: TextIOWrapper, is_valid_symbol: Callable[[str], bool]
    ) -> tuple[
        list[tuple[int, int]],
        dict[tuple[int, int], int],
        list[list[tuple[int, int]]],
        list[int],
    ]:
        """
        Get all the valid symbol positions and number positions.
        """
        symbols = []
        positions_to_id = {}
        id_to_positions = []
        id_to_num = []

        for i, line in enumerate(file):
            num = -1
            num_positions = []
            for j, c in enumerate(line.strip()):
                if c.isdigit():
                    if num == -1:
                        num = 0
                    num = num * 10 + int(c)
                    num_positions.append((i, j))
                    continue

                if num != -1:
                    for pos in num_positions:
                        positions_to_id[pos] = len(id_to_num)
                    id_to_positions.append(num_positions)
                    id_to_num.append(num)

                num = -1
                num_positions = []

                if is_valid_symbol(c):
                    symbols.append((i, j))

            if num != -1:
                for pos in num_positions:
                    positions_to_id[pos] = len(id_to_num)
                id_to_positions.append(num_positions)
                id_to_num.append(num)

        return symbols, positions_to_id, id_to_positions, id_to_num

    def get_adjacent_points(
        self, i: int, j: int
    ) -> Generator[tuple[int, int], None, None]:
        """
        Yields adjacent points.
        """
        for d_i, d_j in itertools.product([-1, 0, 1], repeat=2):
            if d_i == d_j == 0:
                continue
            yield i + d_i, j + d_j

    def product(self, nums: Iterable[int]) -> int:
        """
        Returns the product of the all the numbers.
        """
        result = 1
        for x in nums:
            result *= x
        return result

    def part_1(self):
        """Part 1 solver."""
        with open(
            self.filepath, "r", encoding=sys.getdefaultencoding()
        ) as file:
            (
                symbols,
                positions_to_id,
                id_to_positions,
                id_to_num,
            ) = self.get_nums_and_symbols(file, lambda x: x not in [".", " "])

        def sum_unique_adjacent_nums(i: int, j: int) -> int:
            result = 0

            for adjacent_point in self.get_adjacent_points(i, j):
                if adjacent_point not in positions_to_id:
                    continue
                num_id = positions_to_id[adjacent_point]
                num = id_to_num[num_id]
                result += num

                # Remove all other the number positions from the dictionary.
                for position in id_to_positions[num_id]:
                    positions_to_id.pop(position)
            return result

        return sum(sum_unique_adjacent_nums(i, j) for i, j in symbols)

    def part_2(self):
        """Part 2 solver."""
        with open(
            self.filepath, "r", encoding=sys.getdefaultencoding()
        ) as file:
            stars, positions_to_id, _, id_to_num = self.get_nums_and_symbols(
                file, lambda x: x == "*"
            )

        return sum(
            self.product((id_to_num[num_id] for num_id in adjacent_nums))
            for i, j in stars
            if (
                len(
                    adjacent_nums := {
                        positions_to_id[n_i, n_j]
                        for n_i, n_j in self.get_adjacent_points(i, j)
                        if (n_i, n_j) in positions_to_id
                    }
                )
                == 2
            )
        )

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
