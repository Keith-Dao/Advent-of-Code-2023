"""
Day 5 solution.
"""
import sys
from io import TextIOWrapper
from typing import Callable, Generator, Generic, Iterable, TypeVar

T = TypeVar("T")


class Solver:
    """Day 5 solver."""

    def __init__(self, filepath: str = "input.txt"):
        self.filepath = filepath

    def template_solve(
        self,
        file: TextIOWrapper,
        prev_nums: set[Generic[T]],
        solve: Callable[
            [set[Generic[T]], set[Generic[T]], int, int, int], None
        ],
    ) -> None:
        """
        Generic template solver to map from seed values to location values.
        """
        file.readline()  # Blank line separating seed and first map
        file.readline()  # Map header line

        line = file.readline()
        while line:
            next_nums = set()
            while line.strip() != "":
                # Line in the form: destination source range
                solve(prev_nums, next_nums, *(int(x) for x in line.split()))
                line = file.readline()

            file.readline()  # Blank line separating maps
            line = file.readline()  # Next map's header line or EOF
            prev_nums |= next_nums

    def part_1(self):
        """Part 1 solver."""

        def solve(
            prev_nums: set[int],
            next_nums: set[int],
            destination: int,
            source: int,
            range_length: int,
        ):
            for num in list(prev_nums):
                if not source <= num < source + range_length:
                    continue

                next_nums.add(destination + num - source)
                prev_nums.remove(num)

        with open(
            self.filepath, "r", encoding=sys.getdefaultencoding()
        ) as file:
            # Get seeds
            prev_nums = set(
                int(x) for x in file.readline().split(": ")[1].split()
            )

            # Solve to find locations
            self.template_solve(file, prev_nums, solve)
        return min(prev_nums)

    def part_2(self):
        """Part 2 solver."""

        def batch(
            sequence: Iterable[int], batch_size: int = 1
        ) -> Generator[tuple[int], None, None]:
            batch_result = []
            for x in sequence:
                batch_result.append(x)
                if len(batch_result) == batch_size:
                    yield tuple(batch_result)
                    batch_result.clear()

        def solve(
            prev_ranges: set[tuple[int, int]],
            next_ranges: set[tuple[int, int]],
            destination: int,
            source: int,
            range_length: int,
        ):
            stack = list(prev_ranges)
            while stack:
                first, last = stack.pop()
                if last < source or source + range_length - 1 < first:
                    continue

                prev_ranges.remove((first, last))
                next_ranges.add(
                    (
                        destination + max(source, first) - source,
                        destination
                        + min(source + range_length - 1, last)
                        - source,
                    )
                )

                if first < source:
                    next_range = first, source - 1
                    stack.append(next_range)
                    prev_ranges.add(next_range)

                if source + range_length - 1 < last:
                    next_range = source + range_length, last
                    stack.append(next_range)
                    prev_ranges.add(next_range)

        with open(
            self.filepath, "r", encoding=sys.getdefaultencoding()
        ) as file:
            # Get seed ranges
            seed_details = file.readline().split(": ")[1].split()
            prev_nums = set(
                (start, start + range_length - 1)
                for start, range_length in batch(
                    (int(c) for c in seed_details), 2
                )
            )

            # Find location ranges
            self.template_solve(file, prev_nums, solve)
        return min(x for x, _ in prev_nums)

    def solve(self) -> None:
        """Runs part 1 and part 2."""
        print("Day 5")
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
