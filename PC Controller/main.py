import sys

import serial
from PyQt5.QtCore import Qt, QDir
from PyQt5.QtGui import QIcon
from PyQt5.QtWidgets import QApplication, QMainWindow, QFileDialog, QMessageBox

from gui.mainwindow import Ui_MainWindow
from src.connection_scanner import ConnectionScanner
from src.controler import Controller
from src.gcode_file import GcodeFile
from src.pyinstaller_helper import PyInstallerHelper

__author__ = "Ivan Sevcik"


class MainWindow(QMainWindow, Ui_MainWindow):
    def __init__(self):
        super(MainWindow, self).__init__()
        self.setupUi(self)
        self.setAttribute(Qt.WA_QuitOnClose)

        self._connection_scanner = ConnectionScanner()
        self.refresh_ports()

        self.refreshButton.clicked.connect(self.refresh_ports)
        self.connectButton.clicked.connect(self.connect)
        self.runButton.clicked.connect(self.run)
        self.inputFileButton.clicked.connect(self.browse_input_file)

        self.disconnected()

        self._controller = None
        self._run_thread = None

    def refresh_ports(self):
        self._connection_scanner.scan_connections(with_wifi=False)
        # Populate port combo box and select default
        self.connectionComboBox.clear()

        if self._connection_scanner.port_list:
            for port in self._connection_scanner.port_list:
                self.connectionComboBox.addItem(port)

            self.connectionComboBox.setCurrentIndex(0)
            self.runButton.setEnabled(True)
        else:
            self.runButton.setEnabled(False)

    def connect(self):
        if self._controller is None:
            try:
                connection = self._connection_scanner.port_list[self.connectionComboBox.currentIndex()]
                self._controller = Controller(connection)
                self.connected()
            except serial.SerialException as e:
                QMessageBox().warning(self, "Serial Error", str(e), QMessageBox.Ok)
                self.disconnected()
        else:
            self._controller = None
            self.disconnected()

    def connected(self):
        self.runButton.setEnabled(True)
        self.connectButton.setText("Disconnect")
        self.statusLabel.setStyleSheet("color:green")

    def disconnected(self):
        self.runButton.setEnabled(False)
        self.connectButton.setText("Connect")
        self.statusLabel.setStyleSheet("color:red")

    def run(self):
        try:
            file_path = self.inputFileEdit.text()
            gcode_file = GcodeFile(file_path)
            self.totalCountLabel.setText(str(len(gcode_file)))
            self.processedCountLabel.setText("0")

            # TODO: Report progress
            # TODO: Ability to stop
            self._controller.run(gcode_file)
        except FileNotFoundError as e:
            QMessageBox().warning(self, "File Error", "Can't open specified file", QMessageBox.Ok)

    def browse_input_file(self):
        path = QFileDialog().getOpenFileName(
            caption="Select external editor",
            options=QFileDialog.ReadOnly)

        if path[0]:
            self.inputFileEdit.setText(path[0])

# Main Function
if __name__ == '__main__':
    # Create main app
    myApp = QApplication(sys.argv)
    myApp.setQuitOnLastWindowClosed(True)

    path = PyInstallerHelper.resource_path("icons\\main.png")

    icon = QIcon(path)
    myApp.setWindowIcon(icon)

    ex2 = MainWindow()
    ex2.show()

    # Execute the Application and Exit
    sys.exit(myApp.exec_())