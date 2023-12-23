"""
Day 23 solution.
"""
import collections
import itertools
import sys


class Solver:
    """Day 23 solver."""

    def __init__(self, filepath: str = "input.txt"):
        self.filepath = filepath

    def get_graph(
        self, ignore_slopes: bool
    ) -> tuple[
        dict[tuple[int, int], dict[tuple[int, int], int]], tuple[int, int]
    ]:
        """
        Parse the file and get a compressed graph with the number of rows
        and columns respectively.
        """
        with open(
            self.filepath, "r", encoding=sys.getdefaultencoding()
        ) as file:
            grid = [line.strip() for line in file]

        slopes = "<^>v"
        m, n = len(grid), len(grid[0])
        graph: dict[
            tuple[int, int], dict[tuple[int, int], int]
        ] = collections.defaultdict(dict)
        for i, row in enumerate(grid):
            for j, x in enumerate(row):
                if x == "#":
                    continue

                for slope_type, (d_i, d_j) in zip(
                    slopes, itertools.pairwise((0, -1, 0, 1, 0))
                ):
                    if (
                        not ignore_slopes
                        and grid[i][j] in slopes
                        and grid[i][j] != slope_type
                    ):
                        continue

                    n_i, n_j = i + d_i, j + d_j
                    if (
                        not 0 <= n_i < m
                        or not 0 <= n_j < n
                        or grid[n_i][n_j] == "#"
                    ):
                        continue

                    graph[i, j][n_i, n_j] = 1

        done = False
        while not done:
            done = True
            for node, neighbours in graph.items():
                if len(neighbours) != 2:
                    continue

                a, b = neighbours
                try:
                    graph[a].pop(node)
                    graph[a][b] = neighbours[a] + neighbours[b]
                except KeyError:
                    # Edge is unidirectional i.e. only node -> a
                    pass

                try:
                    graph[b].pop(node)
                    graph[b][a] = neighbours[a] + neighbours[b]
                except KeyError:
                    # Edge is unidirectional i.e. only node -> b
                    pass

                graph.pop(node)
                done = False
                break

        return graph, (m, n)

    def generic_solve(self, ignore_slopes: bool) -> int:
        """Generic solve."""
        graph, (m, n) = self.get_graph(ignore_slopes)

        stack: list[tuple[int, int, int]] = [(0, 1, 0)]
        visited: list[list[bool]] = [[False] * n for _ in range(m)]
        result = 0
        while stack:
            i, j, total = stack.pop()
            if total == -1:
                visited[i][j] = False
                continue

            if i == m - 1:
                result = max(result, total)
                continue

            visited[i][j] = True
            stack.append((i, j, -1))
            for (n_i, n_j), cost in graph[i, j].items():
                if visited[n_i][n_j]:
                    continue

                stack.append((n_i, n_j, total + cost))

        return result

    def part_1(self) -> int:
        """Part 1 solver."""
        return self.generic_solve(False)

    def part_2(self) -> int:
        """Part 2 solver."""
        return self.generic_solve(True)

    def solve(self) -> None:
        """Runs part 1 and part 2."""
        print("Day 23")
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
