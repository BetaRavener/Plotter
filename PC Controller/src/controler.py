import sys
import serial


class Controller:
    BUFFER_SIZE = 5

    def __init__(self, port):
        self._serial_connection = serial.Serial(port, 57600, timeout=0, dsrdtr=False)
        self._input_lines = []
        self._input_line_idx = 0
        self._processing_line_idx = -1
        self._recv_buffer = ""

    def _send_line(self, line):
        line += "\n"
        print("Sent: " + line, end="")
        self._serial_connection.write(line.encode("utf-8"))

    def _send_command(self, line_idx):
        line = "{}*{}".format(line_idx, self._input_lines[line_idx].rstrip())
        line = "{}*{}".format(line, self._checksum(line))
        self._send_line(line)

    def _process_response(self):
        # Read everything from serial
        while True:
            s = self._serial_connection.read(100)
            if not s:
                break
            s = s.decode("utf-8")
            print(s, end="")
            self._recv_buffer += s

        while True:
            ln_idx = self._recv_buffer.find("\n")
            if ln_idx < 0:
                break
            msg = self._recv_buffer[0:ln_idx]
            self._recv_buffer = self._recv_buffer[ln_idx+1:]
            # Remove potential \r character
            msg.rstrip()
            if not msg:
                continue

            # Ignore messages starting with !
            if msg[0] == "!":
                continue

            items = msg.split("*")
            # Check for correct number of fields
            if len(items) != 3:
                continue

            # Check checksum
            csum = int(items[2])
            if self._checksum("{}*{}".format(items[0], items[1])) != csum:
                continue

            line_idx = int(items[0])
            if items[1] == "ok":
                if line_idx > self._processing_line_idx:
                    self._processing_line_idx = line_idx
            elif items[1] == "resend":
                self._input_line_idx = line_idx

    def _send_job(self):
        self._send_line("new")
        while True:
            recv = self._serial_connection.readline()
            if recv and recv.decode("utf-8").rstrip() == "ready":
                break

        while self._input_line_idx != len(self._input_lines):
            if self._input_line_idx - self._processing_line_idx <= Controller.BUFFER_SIZE:
                self._send_command(self._input_line_idx)
                self._input_line_idx += 1
            else:
                self._process_response()
        self._send_line("end")

    @staticmethod
    def _checksum(s):
        csum = 0
        for c in s:
            csum ^= ord(c)
        return csum

    def send_file(self, file_path):
        with open(file_path) as file:
            self._input_lines = file.readlines()
            self._input_line_idx = 0
            self._processing_line_idx = -1
            self._recv_buffer = ""
            self._send_job()
