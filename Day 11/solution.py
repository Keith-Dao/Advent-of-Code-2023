"""
Day 11 solution.
"""
import bisect
import itertools
import sys


class Solver:
    """Day 11 solver."""

    def __init__(self, filepath: str = "input.txt"):
        self.filepath = filepath

    @staticmethod
    def distance(
        galaxy_1: tuple[int, int],
        galaxy_2: tuple[int, int],
        empty_axis_positions: tuple[list[int], list[int]],
        expansion_factor: int,
    ) -> int:
        """
        Calculate the manhattan distance with expanding empty rows
        or columns by the expansion factor.
        """
        return sum(
            abs(a - b)
            + (
                bisect.bisect_right(empty_positions, max(a, b))
                - bisect.bisect_left(empty_positions, min(a, b))
            )
            * (expansion_factor - 1)
            for a, b, empty_positions in zip(
                galaxy_1, galaxy_2, empty_axis_positions
            )
        )

    def generic_solve(self, expansion_factor: int) -> int:
        """Generic solver."""
        galaxies: list[tuple[int, int]] = []
        m = n = 0
        with open(
            self.filepath, "r", encoding=sys.getdefaultencoding()
        ) as file:
            for i, line in enumerate(file):
                for j, x in enumerate(line.strip()):
                    if x == "#":
                        galaxies.append((i, j))
                n = len(line)
                m += 1

        non_empty_rows = {i for i, _ in galaxies}
        non_empty_cols = {j for _, j in galaxies}

        empty_rows = [i for i in range(m) if i not in non_empty_rows]
        empty_cols = [j for j in range(n) if j not in non_empty_cols]

        return sum(
            self.distance(
                *galaxy_pair, (empty_rows, empty_cols), expansion_factor
            )
            for galaxy_pair in itertools.combinations(galaxies, 2)
        )

    def part_1(self) -> int:
        """Part 1 solver."""
        return self.generic_solve(2)

    def part_2(self) -> int:
        """Part 2 solver."""
        return self.generic_solve(1000000)

    def solve(self) -> None:
        """Runs part 1 and part 2."""
        print("Day 11")
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
