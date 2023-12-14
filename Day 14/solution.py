"""
Day 14 solution.
"""
import enum
import sys
from typing import Iterable


class Solver:
    """Day 14 solver."""

    def __init__(self, filepath: str = "input.txt"):
        self.filepath = filepath

    class symbols(enum.Enum):
        """Valid input symbols."""

        SPACES = "."
        ROUND_ROCKS = "O"
        CUBED_ROCKS = "#"

    def parse_file(
        self,
    ) -> tuple[list[list[int]], list[list[int]], int]:
        """
        Parse the file to get the round rocks and the cubed rocks.
        """
        round_rocks = []
        cubed_rocks = []
        with open(
            self.filepath, "r", encoding=sys.getdefaultencoding()
        ) as file:
            for i, line in enumerate(file):
                round_rocks.append([])
                cubed_rocks.append([])

                for j, c in enumerate(line.strip()):
                    if c == self.symbols.SPACES.value:
                        continue

                    if c == self.symbols.ROUND_ROCKS.value:
                        round_rocks[i].append(j)
                    elif c == self.symbols.CUBED_ROCKS.value:
                        cubed_rocks[i].append(j)
                    else:
                        raise ValueError(
                            f"Expected chars to either be '.', '#', or 'O', got '{c}'."
                        )
                n = len(line.strip())
        return round_rocks, cubed_rocks, n

    @staticmethod
    def transpose_row_list(
        row_list: list[list[int]], new_length: int
    ) -> list[list[int]]:
        """Transpose the graph."""
        if not row_list:
            raise ValueError("Cannot transpose empty graph.")

        result = [[] for _ in range(new_length)]
        for i, row in enumerate(row_list):
            for j in row:
                result[j].append(i)
        return result

    @staticmethod
    def tilt_north(
        round_rocks: list[list[int]],
        cubed_rocks: list[list[int]],
        num_cols: int,
    ) -> list[list[int]]:
        """Tilt the map north."""
        new_round_rocks = [[] for _ in range(len(round_rocks))]
        round_rocks = Solver.transpose_row_list(round_rocks, num_cols)
        cubed_rocks = Solver.transpose_row_list(cubed_rocks, num_cols)

        for col, (rounds, cubes) in enumerate(zip(round_rocks, cubed_rocks)):
            i = 0
            last_row = -1
            for round in rounds:
                while i < len(cubes) and cubes[i] < round:
                    last_row = cubes[i]
                    i += 1
                last_row += 1
                new_round_rocks[last_row].append(col)
        return new_round_rocks

    @staticmethod
    def tilt_west(
        round_rocks: list[list[int]], cubed_rocks: list[list[int]], _: int
    ) -> list[list[int]]:
        """Tilt the map west."""
        new_round_rocks = [[] for _ in range(len(round_rocks))]

        for row, (rounds, cubes) in enumerate(zip(round_rocks, cubed_rocks)):
            i = 0
            last_col = -1
            for round in rounds:
                while i < len(cubes) and cubes[i] < round:
                    last_col = cubes[i]
                    i += 1
                last_col += 1
                new_round_rocks[row].append(last_col)
        return new_round_rocks

    @staticmethod
    def tilt_south(
        round_rocks: list[list[int]],
        cubed_rocks: list[list[int]],
        num_cols: int,
    ) -> list[list[int]]:
        """Tilt the map south."""
        num_rows = len(round_rocks)
        new_round_rocks = [[] for _ in range(len(round_rocks))]
        round_rocks = Solver.transpose_row_list(round_rocks, num_cols)
        cubed_rocks = Solver.transpose_row_list(cubed_rocks, num_cols)

        for col, (rounds, cubes) in enumerate(zip(round_rocks, cubed_rocks)):
            i = len(cubes) - 1
            last_row = num_rows
            for round in reversed(rounds):
                while i >= 0 and cubes[i] > round:
                    last_row = cubes[i]
                    i -= 1
                last_row -= 1
                new_round_rocks[last_row].append(col)
        return new_round_rocks

    @staticmethod
    def tilt_east(
        round_rocks: list[list[int]],
        cubed_rocks: list[list[int]],
        num_cols: int,
    ) -> list[list[int]]:
        """Tilt the map east."""
        new_round_rocks = [[] for _ in range(len(round_rocks))]

        for row, (rounds, cubes) in enumerate(zip(round_rocks, cubed_rocks)):
            i = len(cubes) - 1
            last_col = num_cols
            for round in reversed(rounds):
                while i >= 0 and cubes[i] > round:
                    last_col = cubes[i]
                    i -= 1
                last_col -= 1
                new_round_rocks[row].append(last_col)
        return new_round_rocks

    @staticmethod
    def row_list_to_key(graph: list[list[int]]) -> tuple[tuple[int, ...], ...]:
        """Convert the graph to a hashable key."""
        return tuple(tuple(row) for row in graph)

    @staticmethod
    def calculate_north_load(round_rocks: Iterable[Iterable[int]]) -> int:
        """Calculate the total north load of the current round rock configuration."""
        return sum(
            len(row) * (len(round_rocks) - i)
            for i, row in enumerate(round_rocks)
        )

    def part_1(self) -> int:
        """Part 1 solver."""
        round_rocks, cubed_rocks, num_cols = self.parse_file()

        round_rocks = self.tilt_north(round_rocks, cubed_rocks, num_cols)
        return self.calculate_north_load(round_rocks)

    def part_2(self) -> int:
        """Part 2 solver."""
        round_rocks, cubed_rocks, num_cols = self.parse_file()

        seen: dict[tuple[tuple[int, ...], ...], int] = {}
        round_to_key: list[tuple[tuple[int, ...], ...]] = []
        cycle = cycle_start = -1

        for k in range(1000000000):
            key = self.row_list_to_key(round_rocks)
            if key in seen:
                cycle_start = seen[key]
                cycle = k - seen[key]
                break
            seen[key] = k
            round_to_key.append(key)

            for tilt in (
                self.tilt_north,
                self.tilt_west,
                self.tilt_south,
                self.tilt_east,
            ):
                round_rocks = tilt(round_rocks, cubed_rocks, num_cols)

        if cycle != -1 and cycle_start != -1:
            round_rocks = round_to_key[
                cycle_start + (1000000000 - cycle_start) % cycle
            ]

        return self.calculate_north_load(round_rocks)

    def solve(self) -> None:
        """Runs part 1 and part 2."""
        print("Day 14")
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
