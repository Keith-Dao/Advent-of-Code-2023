"""
Day 22 solution.
"""
import collections
import heapq
import itertools
import sys

X = Y = Z = int
ZYX = tuple[Z, Y, X]
"""Coordinates for the z, y, z axes respectively."""
XYZ = tuple[X, Y, Z]
"""Coordinates for the x, y, z axes respectively."""


class Solver:
    """Day 22 solver."""

    def __init__(self, filepath: str = "input.txt"):
        self.filepath = filepath

    def get_resting_bricks(
        self,
    ) -> tuple[list[tuple[XYZ, XYZ]], dict[XYZ, int]]:
        """Get the bricks after they are at rest and the blocks that the occupied by a brick."""

        def parse_coordinate(coordinate: str) -> ZYX:
            x, y, z = [int(x) for x in coordinate.split(",")]
            return z, y, x

        def parse_line(
            line: str,
        ) -> tuple[ZYX, ZYX]:
            start_coord, end_coord = (
                parse_coordinate(coordinate)
                for coordinate in line.strip().split("~")
            )
            lengths = tuple(e - s + 1 for s, e in zip(start_coord, end_coord))
            assert len(lengths) == 3
            return start_coord, lengths

        with open(
            self.filepath, "r", encoding=sys.getdefaultencoding()
        ) as file:
            bricks = [parse_line(line) for line in file]

        # Find the resting positions for the bricks
        heapq.heapify(bricks)
        lowest_non_blocking_z: collections.defaultdict[
            tuple[X, Y], Z
        ] = collections.defaultdict(int)
        resting_bricks: list[tuple[XYZ, XYZ]] = []
        blocks_to_id: dict[XYZ, int] = {}
        while bricks:
            (z, y, x), lengths = heapq.heappop(bricks)
            l_z, l_y, l_x = lengths

            # Find the z position the brick will rest at
            max_blocking_z = 1 + max(
                lowest_non_blocking_z[x + d_x, y + d_y]
                for d_x, d_y in itertools.product(range(l_x), range(l_y))
            )
            min_z = min(max_blocking_z, z)

            # Mark the new highest z positions for each of the brick's xy positions
            for d_x, d_y in itertools.product(range(l_x), range(l_y)):
                n_x, n_y = (
                    x + d_x,
                    y + d_y,
                )
                lowest_non_blocking_z[n_x, n_y] = max(
                    lowest_non_blocking_z[n_x, n_y], min_z + l_z - 1
                )

            # Mark the blocks to belong to the current brick
            for d_z, d_y, d_x in itertools.product(
                *[range(l) for l in lengths]
            ):
                blocks_to_id[x + d_x, y + d_y, min_z + d_z] = len(
                    resting_bricks
                )

            resting_bricks.append(((x, y, min_z), (l_x, l_y, l_z)))

        return resting_bricks, blocks_to_id

    @staticmethod
    def create_directional_support_graphs(
        resting_bricks: list[tuple[XYZ, XYZ]], blocks_to_id: dict[XYZ, int]
    ) -> tuple[list[set[int]], list[set[int]]]:
        """
        Creates directional graphs mapping:
            - a brick to all the bricks it is supports
            - a brick to all the bricks it is supported by
        """
        supports = [set() for _ in range(len(resting_bricks))]
        supported_by = [set() for _ in range(len(resting_bricks))]
        for brick, ((x, y, z), (l_x, l_y, _)) in enumerate(resting_bricks):
            for d_x, d_y in itertools.product(range(l_x), range(l_y)):
                try:
                    brick_below = blocks_to_id[x + d_x, y + d_y, z - 1]
                except KeyError:
                    continue

                supported_by[brick].add(brick_below)
                supports[brick_below].add(brick)

        return supports, supported_by

    def part_1(self) -> int:
        """Part 1 solver."""
        resting_bricks, blocks_to_id = self.get_resting_bricks()

        supports, supported_by = self.create_directional_support_graphs(
            resting_bricks, blocks_to_id
        )

        return sum(
            all(
                len(supported_by[supported_brick]) > 1
                for supported_brick in supported_bricks
            )
            for supported_bricks in supports
        )

    def part_2(self) -> int:
        """Part 2 solver."""
        resting_bricks, blocks_to_id = self.get_resting_bricks()

        supports, supported_by = self.create_directional_support_graphs(
            resting_bricks, blocks_to_id
        )

        def count_falling_bricks(supported_bricks: set[int]) -> int:
            stack = [
                brick
                for brick in supported_bricks
                if len(supported_by[brick]) == 1
            ]

            falling_bricks = set(stack)
            while stack:
                brick = stack.pop()
                for candidate_brick in supports[brick]:
                    if candidate_brick in falling_bricks:
                        continue

                    if any(
                        support_brick not in falling_bricks
                        for support_brick in supported_by[candidate_brick]
                    ):
                        continue

                    falling_bricks.add(candidate_brick)
                    stack.append(candidate_brick)

            return len(falling_bricks)

        return sum(
            count_falling_bricks(supported_bricks)
            for supported_bricks in supports
        )

    def solve(self) -> None:
        """Runs part 1 and part 2."""
        print("Day 22")
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
