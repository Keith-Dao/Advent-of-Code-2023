"""
Day 24 solution.
"""
import itertools
import sys


class Solver:
    """Day 24 solver."""

    def __init__(self, filepath: str = "input.txt"):
        self.filepath = filepath

    def get_hailstones(
        self,
    ) -> list[tuple[tuple[int, int, int], tuple[int, int, int]]]:
        """Get the hailstones from the input file."""

        def split_coords(coordinates: str) -> tuple[int, int, int]:
            x, y, z = [int(x) for x in coordinates.split(", ")]
            return x, y, z

        def parse_line(
            line: str,
        ) -> tuple[tuple[int, int, int], tuple[int, int, int]]:
            positions, velocities = line.split(" @ ")
            return split_coords(positions), split_coords(velocities)

        with open(
            self.filepath, "r", encoding=sys.getdefaultencoding()
        ) as file:
            return [parse_line(line) for line in file]

    def part_1(self) -> int:
        """Part 1 solver."""
        hailstones = self.get_hailstones()
        LOWER_BOUND = 200000000000000
        UPPER_BOUND = 400000000000000

        def intersects(i, j):
            (x_0, y_0, _), (vx_0, vy_0, _) = hailstones[i]
            (x_1, y_1, _), (vx_1, vy_1, _) = hailstones[j]

            try:
                x = (
                    vx_1 * vx_0 * (y_0 - y_1)
                    + vy_1 * vx_0 * x_1
                    - vy_0 * vx_1 * x_0
                ) / (vy_1 * vx_0 - vy_0 * vx_1)
            except ZeroDivisionError:
                return False

            if any((x - p) / v <= 0 for p, v in ((x_0, vx_0), (x_1, vx_1))):
                return False

            y = y_0 + vy_0 * (x - x_0) / vx_0

            return (
                LOWER_BOUND <= x <= UPPER_BOUND
                and LOWER_BOUND <= y <= UPPER_BOUND
            )

        n = len(hailstones)
        return sum(intersects(i, j) for i in range(n) for j in range(i))

    def part_2(self) -> int:
        """Part 2 solver."""
        hailstones = self.get_hailstones()

        # See
        # https://www.reddit.com/r/adventofcode/comments/18pnycy/comment/keqf8uq
        potential_axes_velocities = [set() for _ in range(3)]
        for (
            (positions_0, velocities_0),
            (positions_1, velocities_1),
        ) in itertools.combinations(hailstones, 2):
            for potential, p_0, v_0, p_1, v_1 in zip(
                potential_axes_velocities,
                positions_0,
                velocities_0,
                positions_1,
                velocities_1,
            ):
                if v_0 != v_1:
                    continue

                difference = p_1 - p_0
                candidate_velocities = {
                    v
                    for v in range(-1000, 1000)
                    if v != v_0 and difference % (v - v_0) == 0
                }

                if not potential:
                    potential |= candidate_velocities
                else:
                    potential &= candidate_velocities

        for potential_values in potential_axes_velocities:
            assert len(potential_values) == 1

        vx, vy, vz = (
            potential.pop() for potential in potential_axes_velocities
        )

        (x_0, y_0, z_0), (vx_0, vy_0, vz_0) = hailstones[0]
        (x_1, y_1, _), (vx_1, vy_1, _) = hailstones[1]
        m_0 = (vy_0 - vy) / (vx_0 - vx)
        m_1 = (vy_1 - vy) / (vx_1 - vx)
        c_0 = y_0 - (m_0 * x_0)
        c_1 = y_1 - (m_1 * x_1)
        x = int((c_1 - c_0) / (m_0 - m_1))
        y = int(m_0 * x + c_0)
        t = (x - x_0) // (vx_0 - vx)
        z = z_0 + (vz_0 - vz) * t

        return x + y + z

    def solve(self) -> None:
        """Runs part 1 and part 2."""
        print("Day 24")
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
