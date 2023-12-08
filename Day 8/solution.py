"""
Day 8 solution.
"""
import itertools
import sys
from typing import Callable


class Solver:
    """Day 8 solver."""

    def __init__(self, filepath: str = "input.txt"):
        self.filepath = filepath

    def parse_order_and_graph(self) -> tuple[str, dict[str, tuple[str, str]]]:
        """
        Parse the file to get the order and build the graph.
        """
        with open(
            self.filepath, "r", encoding=sys.getdefaultencoding()
        ) as file:
            order = file.readline().strip()
            file.readline()  # Blank lines

            graph = {
                node: tuple(options.strip()[1:-1].split(", "))
                for node, options in (line.split(" = ") for line in file)
            }

            return order, graph

    @staticmethod
    def get_steps(
        order: str,
        node: str,
        graph: dict[str, tuple[str, str]],
        is_end_node: Callable[[str], bool],
    ) -> int:
        """Count the number of steps to reach the end node."""
        steps = 0
        for direction in itertools.cycle(order):
            node = graph[node][0 if direction == "L" else 1]
            steps += 1
            if is_end_node(node):
                return steps
        return -1

    @staticmethod
    def gcd(a: int, b: int) -> int:
        """The greatest common divisor."""
        if a < b:
            a, b = b, a

        while b != 0:
            a, b = b, a % b
        return a

    @staticmethod
    def lcm(nums: list[int]) -> int:
        """The lowest common multiple."""
        while len(nums) > 1:
            a, b = nums.pop(), nums.pop()
            nums.append(a * b // Solver.gcd(a, b))
        return nums[0]

    def part_1(self) -> int:
        """Part 1 solver."""
        order, graph = self.parse_order_and_graph()

        start_node = "AAA"
        if start_node not in graph:
            return -1

        return self.get_steps(
            order, start_node, graph, lambda node: node == "ZZZ"
        )

    def part_2(self) -> int:
        """Part 2 solver."""
        order, graph = self.parse_order_and_graph()

        return self.lcm(
            [
                self.get_steps(
                    order, node, graph, lambda node: node[-1] == "Z"
                )
                for node in graph
                if node[-1] == "A"
            ]
        )

    def solve(self) -> None:
        """Runs part 1 and part 2."""
        print("Day 8")
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
