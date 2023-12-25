"""
Day 25 solution.
"""
import collections
import sys


class Solver:
    """Day 25 solver."""

    def __init__(self, filepath: str = "input.txt"):
        self.filepath = filepath

    def part_1(self) -> int:
        """Part 1 solver."""
        graph = collections.defaultdict(dict)
        with open(
            self.filepath, "r", encoding=sys.getdefaultencoding()
        ) as file:
            for line in file:
                src, rest = line.split(": ")
                for next_ in rest.split():
                    graph[src][next_] = 1
                    graph[next_][src] = 1

        # See details at:
        # https://www.reddit.com/r/adventofcode/comments/18qbsxs/comment/keuadf5

        def bfs(graph, parents, src, target) -> bool:
            for node in graph:
                parents[node] = None
            parents[src] = src
            queue = collections.deque([src])
            while queue:
                node = queue.popleft()
                for next_, capacity in graph[node].items():
                    if capacity <= 0 or parents[next_] is not None:
                        continue
                    parents[next_] = node
                    queue.append(next_)

            return parents[target] is not None

        def min_cuts(graph, parents, src, target) -> float:
            for neighbours in graph.values():
                for u in neighbours:
                    neighbours[u] = 1

            max_flow: float = 0
            while bfs(graph, parents, src, target):
                flow = float("inf")
                node = target
                while node != src:
                    flow = min(flow, graph[parents[node]][node])
                    node = parents[node]

                max_flow += flow

                node = target
                while node != src:
                    prev = parents[node]
                    graph[prev][node] -= flow
                    graph[node][prev] += flow
                    node = prev

            return max_flow

        parents = {node: None for node in graph}
        src, *other = graph
        for target in other:
            if min_cuts(graph, parents, src, target) == 3:
                break

        component_1_size = sum(p is not None for p in parents.values())
        return (len(graph) - component_1_size) * component_1_size

    def part_2(self) -> int:
        """Part 2 solver."""
        return -1

    def solve(self) -> None:
        """Runs part 1 and part 2."""
        print("Day 25")
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
