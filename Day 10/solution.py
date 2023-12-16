"""
Day 10 solution.
"""
import collections
import enum
import sys


class Solver:
    """Day 10 solver."""

    class Directions(enum.Enum):
        """Possible move directions"""

        NORTH = (-1, 0)
        SOUTH = (1, 0)
        EAST = (0, 1)
        WEST = (0, -1)

    def __init__(self, filepath: str = "input.txt"):
        self.filepath = filepath

    def generate_graph(
        self,
    ) -> tuple[
        dict[tuple[int, int], list[tuple[int, int]]],
        tuple[int, int],
        set[tuple[int, int]],
        int,
        int,
    ]:
        """
        Generate the graph.
        """
        graph = {}
        pipes = {
            "|": [self.Directions.SOUTH.value, self.Directions.NORTH.value],
            "-": [self.Directions.EAST.value, self.Directions.WEST.value],
            "L": [self.Directions.NORTH.value, self.Directions.EAST.value],
            "J": [self.Directions.NORTH.value, self.Directions.WEST.value],
            "7": [self.Directions.SOUTH.value, self.Directions.WEST.value],
            "F": [self.Directions.SOUTH.value, self.Directions.EAST.value],
        }
        spaces = set()
        m = n = 0
        start = -1, -1
        with open(
            self.filepath, "r", encoding=sys.getdefaultencoding()
        ) as file:
            for i, line in enumerate(file):
                for j, x in enumerate(line.strip()):
                    if x == ".":
                        spaces.add((i, j))
                    elif x == "S":
                        start = i, j
                    else:
                        graph[i, j] = pipes[x]
                n = len(line)
                m += 1

        # Determine start pipe directions
        graph[start] = []
        start_i, start_j = start
        for d_i, d_j in (direction.value for direction in self.Directions):
            next_node = start_i + d_i, start_j + d_j
            if next_node not in graph:
                continue

            if (-d_i, -d_j) in graph[next_node]:
                graph[start].append((d_i, d_j))

        return graph, start, spaces, m, n

    @staticmethod
    def traverse_loop(
        graph: dict[tuple[int, int], list[tuple[int, int]]],
        start: tuple[int, int],
    ) -> tuple[int, set[tuple[int, int]]]:
        """
        Traverse the loop returning the depth from the start node and all nodes in the loop.
        """
        queue = collections.deque([start])
        loop_nodes = set(queue)
        steps = 0
        while queue:
            for _ in range(len(queue)):
                i, j = queue.popleft()
                for d_i, d_j in graph[i, j]:
                    next_node = i + d_i, j + d_j
                    if next_node not in loop_nodes and next_node in graph:
                        queue.append(next_node)
                        loop_nodes.add(next_node)
            steps += 1

        return steps - 1, loop_nodes

    def part_1(self) -> int:
        """Part 1 solver."""
        graph, start, *_ = self.generate_graph()
        return self.traverse_loop(graph, start)[0]

    def part_2(self) -> int:
        """Part 2 solver."""
        graph, start, spaces, m, n = self.generate_graph()

        _, loop_nodes = self.traverse_loop(graph, start)

        # Consider all pipes not in the loop as a space
        spaces |= {node for node in graph if node not in loop_nodes}

        # Enlarge map so a 1x1 element is 3x3
        visited = {
            (i * 3 + 1 + d_i, j * 3 + 1 + d_j)
            for i, j in loop_nodes
            for d_i, d_j in graph[i, j] + [(0, 0)]
        }

        def flood_mark_outside_nodes(
            node: tuple[int, int],
            visited: set[tuple[int, int]],
            spaces: set[tuple[int, int]],
        ) -> None:
            if node in visited:
                return
            visited.add(node)

            stack = [node]
            while stack:
                i, j = stack.pop()
                if not 0 <= i < m * 3 or not 0 <= j < n * 3:
                    continue

                if i % 3 == 1 and j % 3 == 1:
                    spaces.discard((i // 3, j // 3))

                for d_i, d_j in (
                    direction.value for direction in self.Directions
                ):
                    next_node = i + d_i, j + d_j
                    if next_node not in visited:
                        stack.append(next_node)
                        visited.add(next_node)

        for i in range(1, 3 * m, 3):
            flood_mark_outside_nodes((i, 1), visited, spaces)
            flood_mark_outside_nodes((i, (n - 1) * 3 + 1), visited, spaces)
        for j in range(1, 3 * n, 3):
            flood_mark_outside_nodes((1, j), visited, spaces)
            flood_mark_outside_nodes(((m - 1) * 3 + 1, j), visited, spaces)

        return len(spaces)

    def solve(self) -> None:
        """Runs part 1 and part 2."""
        print("Day 10")
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
