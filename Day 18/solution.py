"""
Day 18 solution.
"""
import sys
from typing import Generator


class Solver:
    """Day 18 solver."""

    def __init__(self, filepath: str = "input.txt"):
        self.filepath = filepath

    LETTER_TO_DIRECTION: dict[str, tuple[int, int]] = {
        "L": (-1, 0),
        "R": (1, 0),
        "D": (0, -1),
        "U": (0, 1),
    }
    NUMBER_TO_DIRECTION: list[tuple[int, int]] = [
        (1, 0),
        (0, -1),
        (-1, 0),
        (0, 1),
    ]

    @staticmethod
    def generic_solve(
        instructions: Generator[tuple[int, tuple[int, int]], None, None]
    ) -> int:
        """Generic solve."""
        area = x_0 = y_0 = edges = 0
        for length, (d_x, d_y) in instructions:
            x_1 = x_0 + length * d_x
            y_1 = y_0 + length * d_y
            # Shoelace formula
            # https://en.m.wikipedia.org/wiki/Shoelace_formula#Triangle_formula
            area += x_1 * y_0 - y_1 * x_0
            edges += length
            x_0, y_0 = x_1, y_1

        # Apply Pick's theorem to find the inner area then re-add the edge.
        # https://en.wikipedia.org/wiki/Pick%27s_theorem#Formula
        return (abs(area) + edges) // 2 + 1

    def part_1(self) -> int:
        """Part 1 solver."""

        def instructions() -> (
            Generator[tuple[int, tuple[int, int]], None, None]
        ):
            with open(
                self.filepath, "r", encoding=sys.getdefaultencoding()
            ) as file:
                for line in file:
                    direction, length, _ = line.split()
                    yield int(length), self.LETTER_TO_DIRECTION[direction]

        return self.generic_solve(instructions())

    def part_2(self) -> int:
        """Part 2 solver."""

        def instructions() -> (
            Generator[tuple[int, tuple[int, int]], None, None]
        ):
            with open(
                self.filepath, "r", encoding=sys.getdefaultencoding()
            ) as file:
                for line in file:
                    instructions = line.split()[2][1:-1]
                    yield int(instructions[1:6], 16), self.NUMBER_TO_DIRECTION[
                        int(instructions[-1])
                    ]

        return self.generic_solve(instructions())

    def solve(self) -> None:
        """Runs part 1 and part 2."""
        print("Day 18")
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
