"""
Day 13 solution.
"""
import sys
from typing import Callable, Generator, Iterable


class Solver:
    """Day 13 solver."""

    def __init__(self, filepath: str = "input.txt"):
        self.filepath = filepath

    def graph_generator(self) -> Generator[list[str], None, None]:
        """
        Parse the input file and generate one graph at a time.
        """
        with open(
            self.filepath, "r", encoding=sys.getdefaultencoding()
        ) as file:
            line = file.readline()
            while line != "":
                graph = []
                while line not in ["\n", ""]:
                    graph.append(line.strip())
                    line = file.readline()

                yield graph
                line = file.readline()

    def generic_solve(
        self, is_valid_reflection: Callable[[list[Iterable[str]], int], bool]
    ) -> int:
        """Generic solve."""
        row_total = col_total = 0
        for graph in self.graph_generator():
            found_reflection = False

            # Check vertical reflection
            for j in range(len(graph[0]) - 1):
                if is_valid_reflection(graph, j):
                    col_total += j + 1
                    found_reflection = True
                    break
            if found_reflection:
                continue

            # Check horizontal reflection
            for i in range(len(graph) - 1):
                if is_valid_reflection(zip(*graph), i):
                    row_total += i + 1
                    break

        return col_total + 100 * row_total

    def part_1(self) -> int:
        """Part 1 solver."""

        def is_valid_reflection(
            reflection_sequences: list[Iterable[str]], reflection_point: int
        ) -> bool:
            return all(
                all(
                    sequence[reflection_point - i]
                    == sequence[reflection_point + i + 1]
                    for i in range(
                        min(  # Get the smaller of the left or right side
                            reflection_point + 1,
                            len(sequence) - reflection_point - 1,
                        )
                    )
                )
                for sequence in reflection_sequences
            )

        return self.generic_solve(is_valid_reflection)

    def part_2(self) -> int:
        """Part 2 solver."""

        def is_valid_reflection(
            sequences: list[Iterable[str]], reflection_point: int
        ) -> bool:
            return (
                sum(
                    sum(
                        sequence[reflection_point - i]
                        != sequence[reflection_point + i + 1]
                        for i in range(
                            min(
                                reflection_point + 1,
                                len(sequence) - reflection_point - 1,
                            )
                        )
                    )
                    for sequence in sequences
                )
                == 1
            )

        return self.generic_solve(is_valid_reflection)

    def solve(self) -> None:
        """Runs part 1 and part 2."""
        print("Day 13")
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
