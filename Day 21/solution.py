"""
Day 21 solution.
"""
import collections
import sys
from typing import Callable


class Solver:
    """Day 21 solver."""

    def __init__(self, filepath: str = "input.txt"):
        self.filepath = filepath

    @staticmethod
    def step(
        graph: list[str],
        queue: collections.deque[tuple[int, int]],
        visited: set[tuple[int, int]],
        should_skip_node: Callable[
            [list[str], set[tuple[int, int]], int, int], bool
        ],
    ) -> None:
        """Simulate one step."""
        visited.clear()

        for _ in range(len(queue)):
            i, j = queue.popleft()
            for d_i, d_j in ((-1, 0), (1, 0), (0, -1), (0, 1)):
                n_i, n_j = i + d_i, j + d_j
                if should_skip_node(graph, visited, n_i, n_j):
                    continue

                visited.add((n_i, n_j))
                queue.append((n_i, n_j))

    def part_1(self) -> int:
        """Part 1 solver."""
        graph = []
        start = -1, -1
        with open(
            self.filepath, "r", encoding=sys.getdefaultencoding()
        ) as file:
            for i, line in enumerate(file):
                graph.append(line.strip())
                try:
                    start = i, line.index("S")
                except ValueError:
                    pass

        queue = collections.deque([start])
        visited = set()

        def should_skip_node(
            graph: list[str], visited: set[tuple[int, int]], i: int, j: int
        ) -> bool:
            return (
                not 0 <= i < len(graph)
                or not 0 <= j < len(graph[0])
                or (i, j) in visited
                or graph[i][j] == "#"
            )

        for _ in range(64):
            self.step(graph, queue, visited, should_skip_node)
        return len(visited)

    def part_2(self) -> int:
        """Part 2 solver."""
        graph = []
        start = -1, -1
        with open(
            self.filepath, "r", encoding=sys.getdefaultencoding()
        ) as file:
            for i, line in enumerate(file):
                graph.append(line.strip())
                try:
                    start = i, line.index("S")
                except ValueError:
                    pass

        n = len(graph)
        target_step = 26501365
        queue = collections.deque([start])
        visited_values: list[int] = []
        visited = set()

        def should_skip_node(
            graph: list[str], visited: set[tuple[int, int]], i: int, j: int
        ) -> bool:
            return (i, j) in visited or graph[i % len(graph)][
                j % len(graph[0])
            ] == "#"

        for k in range(3):
            for _ in range((target_step % n) if k == 0 else n):
                self.step(graph, queue, visited, should_skip_node)
            visited_values.append(len(visited))

        # See
        # https://en.wikipedia.org/wiki/Newton_polynomial#Newton_forward_divided_difference_formula
        # y = visited_values, s = num_cycles, h = 1
        num_cycles = target_step // n
        divided_difference = [
            visited_values[0],
            visited_values[1] - visited_values[0],
            (
                (visited_values[2] - visited_values[1])
                - (visited_values[1] - visited_values[0])
            )
            // 2,
        ]
        return (
            divided_difference[0]
            + divided_difference[1] * num_cycles
            + num_cycles * (num_cycles - 1) * divided_difference[2]
        )

    def solve(self) -> None:
        """Runs part 1 and part 2."""
        print("Day 21")
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
