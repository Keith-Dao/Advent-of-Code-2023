"""
Day 21 solution.
"""
import collections
import itertools
import sys


class Solver:
    """Day 21 solver."""

    def __init__(self, filepath: str = "input.txt"):
        self.filepath = filepath

    def get_graph(self) -> tuple[list[str], tuple[int, int]]:
        """Get the graph from the input file."""
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
        return graph, start

    @staticmethod
    def step(
        graph: list[str],
        queue: collections.deque[tuple[int, int]],
        visited: set[tuple[int, int]],
    ) -> None:
        """Simulate one step."""
        visited.clear()

        for _ in range(len(queue)):
            i, j = queue.popleft()
            for d_i, d_j in itertools.pairwise((0, -1, 0, 1, 0)):
                n_i, n_j = i + d_i, j + d_j
                if (
                    not 0 <= n_i < len(graph)
                    or not 0 <= n_j < len(graph[0])
                    or (n_i, n_j) in visited
                    or graph[n_i][n_j] == "#"
                ):
                    continue

                visited.add((n_i, n_j))
                queue.append((n_i, n_j))

    def part_1(self) -> int:
        """Part 1 solver."""
        graph, start = self.get_graph()

        queue = collections.deque([start])
        visited = set()

        for _ in range(64):
            self.step(graph, queue, visited)
        return len(visited)

    def part_2(self) -> int:
        """Part 2 solver."""
        graph, start = self.get_graph()

        m, n = len(graph), len(graph[0])
        target_step = 26501365

        # See https://www.reddit.com/r/adventofcode/comments/18nevo3/comment/keam21w
        # for a better explanation
        # Find all positions that can be reached
        queue = collections.deque([start])
        reachable = {start}
        while queue:
            i, j = queue.popleft()
            for d_i, d_j in itertools.pairwise((0, -1, 0, 1, 0)):
                n_i, n_j = i + d_i, j + d_j
                if (
                    not 0 <= n_i < m
                    or not 0 <= n_j < n
                    or (n_i, n_j) in reachable
                    or graph[n_i][n_j] == "#"
                ):
                    continue
                reachable.add((n_i, n_j))
                queue.append((n_i, n_j))

        # Find the inner diamond positions
        queue = collections.deque([start])
        visited = set()
        for _ in range(target_step % m):
            self.step(graph, queue, visited)
        inner_odd_positions = set(visited)
        self.step(graph, queue, visited)
        inner_even_positions = set(visited)
        num_repeats = (
            2 * target_step
        ) // m + 1  # Number of tiles along an axis

        outer_positions = (
            reachable - inner_even_positions - inner_odd_positions
        )

        num_even_tiles, num_odd_tiles = sorted(
            (num_repeats // 2, num_repeats - num_repeats // 2)
        )

        return (
            num_odd_tiles * num_odd_tiles * len(inner_odd_positions)
            + num_odd_tiles * num_even_tiles * len(outer_positions)
            + num_even_tiles * num_even_tiles * len(inner_even_positions)
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
