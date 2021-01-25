from PyQt5.QtWidgets import QApplication
from mainapp import MainApp
import numpy as np
import sys

app = QApplication(sys.argv)
frame_rect = app.desktop().frameGeometry()
wsize = (frame_rect.width(), frame_rect.height())
window = MainApp(wsize)
window.show()
sys.exit(app.exec_())
