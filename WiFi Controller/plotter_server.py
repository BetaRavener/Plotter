from ws_connection import ClientClosedError
from ws_server import WebSocketServer, WebSocketClient
import time
import machine


class PlotterClient(WebSocketClient):
    def __init__(self, conn, serial):
        super().__init__(conn)
        self.serial = serial

    def process(self):
        try:
            msg = self.connection.read()
            if msg:
                self.serial.write(msg)
            msg = self.serial.readline()
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
