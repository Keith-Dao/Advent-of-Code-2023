"""
Day 2 solution.
"""
import sys
from typing import Generator


class Solver:
    """Day 2 solver."""

    def __init__(self, filepath: str = "input.txt"):
        self.filepath = filepath

    def decode_line(self, line: str) -> tuple[int, list[str]]:
        """
        Decode line to get game id and draw details for the game.
        """
        game_detail, draw_details = line.split(":")
        return (int(game_detail.split()[-1]), draw_details.split(";"))

    def decode_draw(
        self, game_set: str
    ) -> Generator[tuple[int, int], None, None]:
        """
        Decodes set details to a list of colour id, number of colour pairs.
        """
        for ball_details in game_set.split(", "):
            num_token, colour_token = ball_details.split()
            yield int(
                num_token
            ), 0 if colour_token == "red" else 1 if colour_token == "green" else 2

    def part_1(self):
        """Part 1 solver."""
        max_colours = [12, 13, 14]

        def all_draws_less_than_max(draws: list[str]) -> bool:
            def draw_less_than_max(draw: str) -> bool:
                return all(
                    num <= max_colours[colour]
                    for num, colour in self.decode_draw(draw)
                )

            return all(draw_less_than_max(draw) for draw in draws)

        with open(
            self.filepath, "r", encoding=sys.getdefaultencoding()
        ) as file:
            return sum(
                game_id
                for game_id, draws in (self.decode_line(line) for line in file)
                if all_draws_less_than_max(draws)
            )

    def part_2(self):
        """Part 2 solver."""

        def get_min_colours(draws: list[str]) -> list[int]:
            colours = [0, 0, 0]
            for draw in draws:
                for num, colour in self.decode_draw(draw):
                    colours[colour] = max(colours[colour], num)
            return colours

        def multiply(colours: list[int]) -> int:
            result = 1
            for x in colours:
                result *= x
            return result

        with open(
            self.filepath, "r", encoding=sys.getdefaultencoding()
        ) as file:
            return sum(
                multiply(get_min_colours(draws))
                for _, draws in (self.decode_line(line) for line in file)
            )

    def solve(self) -> None:
        """Runs part 1 and part 2."""
        print("Day 2")
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
