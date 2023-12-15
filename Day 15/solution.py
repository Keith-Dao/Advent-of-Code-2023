"""
Day 15 solution.
"""
import collections
import sys
from typing import Generator, Iterable


class Solver:
    """Day 15 solver."""

    def __init__(self, filepath: str = "input.txt"):
        self.filepath = filepath

    def sequence_generator(
        self,
    ) -> Generator[Generator[str, None, None], None, None]:
        """Yields a generator of a sequence's tokens excluding the delimiter."""

        def token_generator() -> Generator[str, None, None]:
            nonlocal done
            c = file.read(1)
            while c not in [",", ""]:
                yield c
                c = file.read(1)

            done = c == ""

        with open(
            self.filepath, "r", encoding=sys.getdefaultencoding()
        ) as file:
            done = False
            while not done:
                yield token_generator()

    @staticmethod
    def get_hash_code(tokens: Iterable[str]) -> int:
        """Calculate the hash code of the tokens."""
        total = 0
        for c in tokens:
            total += ord(c)
            total *= 17
            total %= 256
        return total

    def part_1(self) -> int:
        """Part 1 solver."""
        return sum(
            self.get_hash_code(sequence)
            for sequence in self.sequence_generator()
        )

    def part_2(self) -> int:
        """Part 2 solver."""
        REMOVE = "-"
        REPLACE = "="

        boxes = [collections.OrderedDict() for _ in range(256)]
        for sequence in self.sequence_generator():
            focal_length = 0
            label = []
            op = ""
            for c in sequence:
                if op != "":
                    focal_length = focal_length * 10 + int(c)
                elif c in (REMOVE, REPLACE):
                    op = c
                else:
                    label.append(c)
            hash_code = self.get_hash_code(label)
            label = "".join(label)

            if op == REMOVE:
                boxes[hash_code].pop(label, None)
            elif op == REPLACE:
                boxes[hash_code][label] = focal_length

        return sum(
            box_num * slot_num * focal_length
            for box_num, row in enumerate(boxes, start=1)
            for slot_num, focal_length in enumerate(row.values(), start=1)
        )

    def solve(self) -> None:
        """Runs part 1 and part 2."""
        print("Day 15")
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
