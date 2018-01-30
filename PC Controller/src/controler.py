import sys
from threading import Thread

import serial


class Controller:
    BUFFER_SIZE = 5

    def __init__(self, port):
        self._serial_connection = serial.Serial(port, 57600, timeout=0, dsrdtr=False)
        self._gcode_file = None
        self._last_acked_line_idx = 0
        self._recv_buffer = None

        self._running = False
        self._thread = None

    def _send_message(self, msg):
        msg += "\n"
        print("Sent: " + msg, end="")
        self._serial_connection.write(msg.encode("utf-8"))

    def _send_command(self, idx, command_line):
        command_line = command_line.rstrip()
        msg = "{}*{}".format(idx, command_line)
        msg = "{}*{}".format(msg, self._checksum(msg))
        self._send_message(msg)

    def _read_all_to_buffer(self):
        while True:
            s = self._serial_connection.read(100)
            if not s:
                break
            s = s.decode("utf-8")
            print(s, end="")
            self._recv_buffer += s

    def _process_response(self):
        self._read_all_to_buffer()

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
                if line_idx > self._last_acked_line_idx:
                    self._last_acked_line_idx = line_idx
            elif items[1] == "resend":
                self._gcode_file.rewind(line_idx)

    def _initialize(self, gcode_file):
        self._gcode_file = gcode_file
        self._last_acked_line_idx = -1
        self._recv_buffer = ""

    def _gcode_job(self):
        # Initialize new job with "new" message
        self._send_message("new")

        # Wait for "ready" response
        while True:
            recv = self._serial_connection.readline()
            if recv and recv.decode("utf-8").rstrip() == "ready":
                break

        # Process by line until end of job
        while self._running and not self._gcode_file.finished():
            job_idx = self._gcode_file.get_position()
            if job_idx - self._last_acked_line_idx <= Controller.BUFFER_SIZE:
                cmd = self._gcode_file.get_line()
                self._send_command(job_idx, cmd)
            else:
                self._process_response()

        # Mark end of communication
        self._send_message("end")

    def run(self, gcode_file):
        # Reinitialize
        self._initialize(gcode_file)

        self._running = True
        self._thread = Thread(target=self._gcode_job)
        self._thread.start()

    def stop(self):
        self._running = False
        self._thread.join()

    @staticmethod
    def _checksum(s):
        csum = 0
        for c in s:
            csum ^= ord(c)
        return csum
