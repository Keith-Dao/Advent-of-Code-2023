"""
Day 23 solution.
"""
import itertools
import sys

sys.setrecursionlimit(10000)


class Solver:
    """Day 23 solver."""

    def __init__(self, filepath: str = "input.txt"):
        self.filepath = filepath

    def part_1(self) -> int:
        """Part 1 solver."""
        with open(
            self.filepath, "r", encoding=sys.getdefaultencoding()
        ) as file:
            graph = [line.strip() for line in file]

        def dfs(i, j, visited) -> int:
            if i == len(graph) - 1:
                return len(visited) - 1

            n_i, n_j = i, j
            if graph[i][j] == ">":
                n_i, n_j = i, j + 1
            elif graph[i][j] == "<":
                n_i, n_j = i, j - 1
            elif graph[i][j] == "^":
                n_i, n_j = i - 1, j
            elif graph[i][j] == "v":
                n_i, n_j = i + 1, j

            if i != n_i or j != n_j:
                if (
                    not 0 <= n_i < len(graph)
                    or not 0 <= n_j < len(graph[0])
                    or graph[n_i][n_j] == "#"
                    or (n_i, n_j) in visited
                ):
                    return 0

                visited.add((n_i, n_j))
                result = dfs(n_i, n_j, visited)
                visited.remove((n_i, n_j))
                return result

            result = 0
            for d_i, d_j in itertools.pairwise((0, -1, 0, 1, 0)):
                n_i, n_j = i + d_i, j + d_j
                if (
                    not 0 <= n_i < len(graph)
                    or not 0 <= n_j < len(graph[0])
                    or graph[n_i][n_j] == "#"
                    or (n_i, n_j) in visited
                ):
                    continue

                visited.add((n_i, n_j))
                result = max(result, dfs(n_i, n_j, visited))
                visited.remove((n_i, n_j))
            return result

        return dfs(0, 1, {(0, 1)})

    def part_2(self) -> int:
        """Part 2 solver."""
        with open(
            self.filepath, "r", encoding=sys.getdefaultencoding()
        ) as file:
            blocks = [line.strip() for line in file]

        m, n = len(blocks), len(blocks[0])
        junctions: list[tuple[int, int]] = []
        for i, row in enumerate(blocks):
            for j, x in enumerate(row):
                if x == "#":
                    continue

                if (
                    sum(
                        0 <= (n_i := i + d_i) < m
                        and 0 <= (n_j := j + d_j) < n
                        and blocks[n_i][n_j] != "#"
                        for d_i, d_j in itertools.pairwise((0, -1, 0, 1, 0))
                    )
                    > 2
                ):
                    junctions.append((i, j))

        junctions.extend([(0, 1), (len(blocks) - 1, len(blocks[-1]) - 2)])
        graph: list[list[dict[tuple[int, int], int]]] = [
            [dict() for _ in row] for row in blocks
        ]
        for node in junctions:
            visited = [[False] * len(row) for row in blocks]
            stack: list[tuple[int, int, int]] = [(*node, 0)]
            while stack:
                i, j, steps = stack.pop()
                if visited[i][j]:
                    continue
                visited[i][j] = True

                if (i, j) in junctions and (i, j) != node:
                    graph[node[0]][node[1]][i, j] = steps
                    continue

                for d_i, d_j in itertools.pairwise((0, -1, 0, 1, 0)):
                    n_i, n_j = i + d_i, j + d_j
                    if (
                        not 0 <= n_i < m
                        or not 0 <= n_j < n
                        or blocks[n_i][n_j] == "#"
                    ):
                        continue

                    stack.append((n_i, n_j, steps + 1))

        result = 0
        visited = [[False] * len(row) for row in blocks]

        def dfs(i: int, j: int, count: int) -> None:
            nonlocal result

            if visited[i][j]:
                return

            if i == len(blocks) - 1:
                result = max(result, count)
                return

            visited[i][j] = True
            for next_, cost in graph[i][j].items():
                dfs(*next_, count + cost)
            visited[i][j] = False

        dfs(0, 1, 0)
        return result

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
