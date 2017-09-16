from machine import UART
import time

#from plotter_server import PlotterServer

SERIAL_SPEED = 9600


from ws_connection import ClientClosedError
from ws_server import WebSocketServer, WebSocketClient
import time
import machine

class PlotterClient(WebSocketClient):
    def __init__(self, conn, serial):
        super().__init__(conn)
        self.serial = serial
        self.idx = 0

    def process(self):
        try:
            msg = self.connection.read()
            if msg:
                self.serial.write(msg)
            msg = self.serial.read()
            if msg:
                self.connection.write(msg)

        except ClientClosedError:
            self.connection.close()


class PlotterServer(WebSocketServer):
    def __init__(self, serial):
        super().__init__("plotter.html", 1)
        self.serial = serial

    def _make_client(self, conn):
        return PlotterClient(conn, self.serial)


def main():
    serial = UART(0, SERIAL_SPEED, timeout=0)

    # Start server
    server = PlotterServer(serial)
    server.start()

    try:
        while True:
            server.process_all()
    except KeyboardInterrupt:
        pass
    server.stop()

main()
