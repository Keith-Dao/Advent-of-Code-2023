"""
Day 17 solution.
"""
import heapq
import sys


class Solver:
    """Day 17 solver."""

    def __init__(self, filepath: str = "input.txt"):
        self.filepath = filepath

    def get_graph(self) -> list[list[int]]:
        """Parse the file and return the graph."""
        with open(
            self.filepath, "r", encoding=sys.getdefaultencoding()
        ) as file:
            return [[int(x) for x in line.strip()] for line in file]

    @staticmethod
    def generic_solve(
        graph: list[list[int]],
        max_steps_without_turning: int,
        min_steps: int = 0,
    ) -> int:
        """Generic solver."""
        m, n = len(graph), len(graph[0])
        visited = [[[-1] * 2 for _ in range(n)] for _ in range(m)]
        heap = [(0, (0, 0, 0)), (0, (0, 0, 1))]

        while heap:
            cost, (i, j, axis) = heapq.heappop(heap)
            if (i, j) == (m - 1, n - 1):
                return cost

            for change in (-1, 1):
                next_cost = cost
                next_i, next_j = i, j
                for steps in range(1, max_steps_without_turning + 1):
                    next_i += (axis == 0) * change
                    next_j += (axis == 1) * change

                    if not 0 <= next_i < m or not 0 <= next_j < n:
                        break
                    next_cost += graph[next_i][next_j]
                    if steps < min_steps:
                        continue

                    next_state = next_i, next_j, 1 - axis
                    if (
                        visited[next_i][next_j][1 - axis] != -1
                        and visited[next_i][next_j][1 - axis] <= next_cost
                    ):
                        continue

                    visited[next_i][next_j][1 - axis] = next_cost
                    heapq.heappush(
                        heap,
                        (next_cost, next_state),
                    )

        return -1

    def part_1(self) -> int:
        """Part 1 solver."""
        return self.generic_solve(self.get_graph(), 3)

    def part_2(self) -> int:
        """Part 2 solver."""
        return self.generic_solve(self.get_graph(), 10, 4)

    def solve(self) -> None:
        """Runs part 1 and part 2."""
        print("Day 17")
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
