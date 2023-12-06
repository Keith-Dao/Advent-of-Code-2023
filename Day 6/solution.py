"""
Day 6 solution.
"""
import sys


class Solver:
    """Day 6 solver."""

    def __init__(self, filepath: str = "input.txt"):
        self.filepath = filepath

    def calculate_distance(self, total_time: int, time_held: int) -> int:
        """
        Calculate the total distance travelled when held for time_held and race is total_time long.
        """
        return time_held * (total_time - time_held)

    def find_minimum_and_maximum_valid_hold_times(
        self, race_time: int, minimum_distance: int
    ) -> tuple[int, int]:
        """
        Finds the maximum and minimum valid hold times.
        """
        # Find minimum valid hold time
        left, right = 0, race_time + 1
        while left < right:
            time = left + (right - left) // 2

            if self.calculate_distance(race_time, time) > minimum_distance:
                right = time
            else:
                left = time + 1
        minimum_hold_time = left

        # Find maximum valid hold time
        right = race_time
        while left < right:
            time = right - (right - left) // 2

            if self.calculate_distance(race_time, time) > minimum_distance:
                left = time
            else:
                right = time - 1
        maximum_hold_time = left

        return minimum_hold_time, maximum_hold_time

    def part_1(self):
        """Part 1 solver."""

        def parse_line(line: str) -> list[int]:
            return [int(x) for x in line.split(":")[1].split()]

        with open(
            self.filepath, "r", encoding=sys.getdefaultencoding()
        ) as file:
            race_times = parse_line(file.readline())
            distances = parse_line(file.readline())

        result = 1
        for race_time, distance in zip(race_times, distances):
            minimum, maximum = self.find_minimum_and_maximum_valid_hold_times(
                race_time, distance
            )
            result *= maximum - minimum + 1

        return result

    def part_2(self):
        """Part 2 solver."""

        def parse_line(line: str) -> int:
            return int("".join(line.split(":")[1].split()))

        with open(
            self.filepath, "r", encoding=sys.getdefaultencoding()
        ) as file:
            race_time = parse_line(file.readline())
            distance = parse_line(file.readline())

        minimum, maximum = self.find_minimum_and_maximum_valid_hold_times(
            race_time, distance
        )
        return maximum - minimum + 1

    def solve(self) -> None:
        """Runs part 1 and part 2."""
        print("Day 6")
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
        case other:
            print("Usage: python solution.py <path>")
            sys.exit(0)
    solver.solve()
