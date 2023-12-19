"""
Day 19 solution.
"""
import sys
from io import TextIOWrapper
from typing import Callable, Iterable

Workflow = dict[str, list[tuple[str, str, int, str]]]


class Solver:
    """Day 19 solver."""

    def __init__(self, filepath: str = "input.txt"):
        self.filepath = filepath

    @staticmethod
    def product(nums: Iterable[int]) -> int:
        """Product of a sequence of numbers."""
        result = 1
        for x in nums:
            result *= x
        return result

    def generic_solve(
        self, solve: Callable[[Workflow, TextIOWrapper], int]
    ) -> int:
        """Generic solve."""
        workflow: Workflow = {}
        with open(
            self.filepath, "r", encoding=sys.getdefaultencoding()
        ) as file:
            for line in file:
                if line == "\n":
                    break

                workflow_name, conditions = line.strip()[:-1].split("{")
                workflow[workflow_name] = []
                for condition_statement in conditions.split(","):
                    if ":" not in condition_statement:
                        workflow[workflow_name].append(
                            ("", "", 0, condition_statement)
                        )
                        continue

                    condition, target = condition_statement.split(":")
                    variable_name, operation, value = (
                        condition[0],
                        condition[1],
                        condition[2:],
                    )
                    workflow[workflow_name].append(
                        (variable_name, operation, int(value), target)
                    )

            return solve(workflow, file)

    def part_1(self) -> int:
        """Part 1 solver."""

        def solve(workflow: Workflow, file: TextIOWrapper) -> int:
            total = 0
            operations = {">": lambda a, b: a > b, "<": lambda a, b: a < b}
            for line in file:
                values = {}
                for statements in line.strip()[1:-1].split(","):
                    variable_name, value = statements.split("=")
                    values[variable_name] = int(value)

                current = "in"
                while current not in ["A", "R"]:
                    for variable_name, operation, value, target in workflow[
                        current
                    ]:
                        if (operation == "") or operations[operation](
                            values[variable_name], value
                        ):
                            current = target
                            break

                if current == "A":
                    total += sum(values.values())

            return total

        return self.generic_solve(solve)

    def part_2(self) -> int:
        """Part 2 solver."""

        def solve(workflow: Workflow, _: TextIOWrapper) -> int:
            def get_new_ranges(
                low: int, high: int, value: int, operation: str
            ) -> tuple[tuple[int, int], tuple[int, int]]:
                if operation == "<":
                    return (low, value - 1), (value, high)
                if operation == ">":
                    return (value + 1, high), (low, value)
                raise ValueError("Invalid operation.")

            def count_unique_arrangements(
                current: str, ranges: dict[str, tuple[int, int]]
            ) -> int:
                if current == "R":
                    return 0

                if current == "A":
                    return self.product(
                        high - low + 1 for low, high in ranges.values()
                    )

                original_ranges = dict(ranges)
                result = 0
                for variable_name, operation, value, target in workflow[
                    current
                ]:
                    if operation == "":
                        result += count_unique_arrangements(target, ranges)
                        continue

                    next_ranges = (
                        positive_range,
                        negative_range,
                    ) = get_new_ranges(
                        *ranges[variable_name], value, operation
                    )
                    if any(low > high for low, high in next_ranges):
                        break

                    ranges[variable_name] = positive_range
                    result += count_unique_arrangements(target, ranges)
                    ranges[variable_name] = negative_range

                for variable_name, range in original_ranges.items():
                    ranges[variable_name] = range
                return result

            return count_unique_arrangements(
                "in", {variable_name: (1, 4000) for variable_name in "xmas"}
            )

        return self.generic_solve(solve)

    def solve(self) -> None:
        """Runs part 1 and part 2."""
        print("Day 19")
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
