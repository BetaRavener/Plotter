class GcodeFile:
    def __init__(self, src_file_path):
        self._gc_lines = None
        self._current_position = 0

        with open(src_file_path) as file:
            self._gc_lines = file.readlines()

        # Filter out comments
        self._gc_lines = [x for x in self._gc_lines if not (x[0] == "(" and x.rstrip()[-1] == ")")]

    def __len__(self):
        return len(self._gc_lines)

    def get_line(self):
        line = self._gc_lines[self._current_position]
        self._current_position += 1
        return line

    def get_position(self):
        return self._current_position

    def rewind(self, pos_idx):
        self._current_position = pos_idx

    def finished(self):
        return self._current_position == len(self._gc_lines)