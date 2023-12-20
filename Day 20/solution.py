"""
Day 20 solution.
"""
from __future__ import annotations

import collections
import enum
import math
import sys


class Solver:
    """Day 20 solver."""

    def __init__(self, filepath: str = "input.txt"):
        self.filepath = filepath

    class module_types(enum.Enum):
        """Value module types."""

        BROADCASTER = "broadcaster"
        FLIP_FLOP = "%"
        CONJUNCTION = "&"

    def parse_file(
        self,
    ) -> tuple[
        dict[str, list[str]],
        dict[str, str],
        dict[str, bool],
        dict[str, dict[str, bool]],
    ]:
        """Parse the file to get the module outputs, types, state and input values."""
        modules_outputs: dict[str, list[str]] = {}
        modules_type: dict[str, str] = {}
        modules_state: dict[str, bool] = collections.defaultdict(bool)
        modules_inputs: dict[str, dict[str, bool]] = collections.defaultdict(
            lambda: collections.defaultdict(bool)
        )

        with open(
            self.filepath, "r", encoding=sys.getdefaultencoding()
        ) as file:
            for line in file:
                module, outputs = line.split(" -> ")
                outputs = outputs.strip().split(", ")

                if module == self.module_types.BROADCASTER.value:
                    modules_type[module] = module
                else:
                    module_type = module[0]
                    module = module[1:]
                    modules_type[module] = module_type

                for output in outputs:
                    modules_inputs[output][module] = False
                modules_state[module] = False
                modules_outputs[module] = outputs

        return modules_outputs, modules_type, modules_state, modules_inputs

    @staticmethod
    def send_pulse(
        modules_type: dict[str, str],
        modules_state: dict[str, bool],
        modules_inputs: dict[str, dict[str, bool]],
        node: str,
        pulse: bool,
        prev: str,
    ) -> bool:
        """
        Compute the sent pulse and determine whether the module
        should send a pulse to all its output modules.
        """
        module_type = modules_type[node]
        if module_type == Solver.module_types.BROADCASTER.value:
            return True

        if module_type not in [
            Solver.module_types.FLIP_FLOP.value,
            Solver.module_types.CONJUNCTION.value,
        ]:
            return False

        if module_type == Solver.module_types.FLIP_FLOP.value:
            if pulse:
                return False

            modules_state[node] ^= True
            return True

        modules_inputs[node][prev] = pulse
        modules_state[node] = any(not x for x in modules_inputs[node].values())
        return True

    def part_1(self) -> int:
        """Part 1 solver."""
        (
            modules_outputs,
            modules_type,
            modules_state,
            modules_inputs,
        ) = self.parse_file()

        pulses = [0, 0]
        for _ in range(1000):
            queue: collections.deque[
                tuple[str, bool, str]
            ] = collections.deque(
                [(self.module_types.BROADCASTER.value, False, "")]
            )
            pulses[False] += 1  # Initial button press

            while queue:
                node, pulse, prev = queue.popleft()

                if not self.send_pulse(
                    modules_type,
                    modules_state,
                    modules_inputs,
                    node,
                    pulse,
                    prev,
                ):
                    continue

                module_state = modules_state[node]
                for next_ in modules_outputs[node]:
                    pulses[module_state] += 1
                    if next_ not in modules_type:
                        continue
                    queue.append((next_, module_state, node))

        return pulses[0] * pulses[1]

    def part_2(self) -> int:
        """Part 2 solver."""
        (
            modules_outputs,
            modules_type,
            modules_state,
            modules_inputs,
        ) = self.parse_file()

        if "rx" not in modules_inputs:
            print("rx module missing from input.")
            return -1

        contingent_nodes = {
            node
            for prev in modules_inputs["rx"]
            for node in modules_inputs[prev]
        }
        cycles = []
        presses = 0
        while contingent_nodes:
            presses += 1
            queue: collections.deque[
                tuple[str, bool, str]
            ] = collections.deque(
                [(self.module_types.BROADCASTER.value, False, "")]
            )
            while queue:
                node, pulse, prev = queue.popleft()

                if node in contingent_nodes and modules_state[node]:
                    contingent_nodes.remove(node)
                    cycles.append(presses)

                if not self.send_pulse(
                    modules_type,
                    modules_state,
                    modules_inputs,
                    node,
                    pulse,
                    prev,
                ):
                    continue

                module_state = modules_state[node]
                for next_ in modules_outputs[node]:
                    if next_ not in modules_type:
                        continue
                    queue.append((next_, module_state, node))

        return math.lcm(*cycles)

    def solve(self) -> None:
        """Runs part 1 and part 2."""
        print("Day 20")
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
