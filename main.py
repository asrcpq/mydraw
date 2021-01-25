from PyQt5.QtCore import QDir, QPoint, QRect, QSize, Qt
from PyQt5.QtGui import QImage, QImageWriter, QPainter, QPen, qRgba, QTabletEvent
from PyQt5.QtWidgets import (QAction, QApplication, QColorDialog, QFileDialog,
		QInputDialog, QMainWindow, QMenu, QMessageBox, QWidget)
from PyQt5.QtPrintSupport import QPrintDialog, QPrinter


class ScribbleArea(QWidget):
	def __init__(self, parent=None):
		super(ScribbleArea, self).__init__(parent)

		self.setAttribute(Qt.WA_StaticContents)
		self.modified = False
		self.scribbling = False
		self.myPenWidth = 1
		self.myPenColor = Qt.red
		self.image = QImage()
		self.lastPoint = QPoint()

		self.setAttribute(Qt.WA_TranslucentBackground)
		self.setWindowOpacity(0.5)

		self.setWindowTitle("Scribble")
		self.resize(500, 500)

	def setPenColor(self, newColor):
		self.myPenColor = newColor

	def setPenWidth(self, newWidth):
		self.myPenWidth = newWidth

	def clearImage(self):
		self.image.fill(qRgba(0, 0, 0, 255))
		self.modified = True
		self.update()

	def tabletEvent(self, tabletEvent):
		self.pos = tabletEvent.pos()
		# self.pen_pressure = int(tabletEvent.pressure() * 100)
		if tabletEvent.type() == QTabletEvent.TabletPress:
			self.pen_is_down = True
			self.text = "TabletPress event"
		elif tabletEvent.type() == QTabletEvent.TabletMove:
			self.pen_is_down = True
			self.text = "TabletMove event"
		elif tabletEvent.type() == QTabletEvent.TabletRelease:
			self.pen_is_down = False
			self.text = "TabletRelease event"
		if self.pen_is_down:
			if not self.scribbling:
				self.lastPoint = self.pos
				self.scribbling = True
			else:
				self.drawLineTo(self.pos)
		else:
			self.drawLineTo(self.pos)
			self.scribbling = False
		tabletEvent.accept()
		self.update()

	def paintEvent(self, event):
		painter = QPainter(self)
		dirtyRect = event.rect()
		painter.drawImage(dirtyRect, self.image, dirtyRect)

	def resizeEvent(self, event):
		if self.width() > self.image.width() or self.height() > self.image.height():
			newWidth = max(self.width() + 128, self.image.width())
			newHeight = max(self.height() + 128, self.image.height())
			self.resizeImage(self.image, QSize(newWidth, newHeight))
			self.update()

		super(ScribbleArea, self).resizeEvent(event)

	def drawLineTo(self, endPoint):
		painter = QPainter(self.image)
		painter.setPen(QPen(self.myPenColor, self.myPenWidth, Qt.SolidLine,
				Qt.RoundCap, Qt.RoundJoin))
		painter.drawLine(self.lastPoint, endPoint)
		self.modified = True

		rad = self.myPenWidth / 2 + 2
		self.update(QRect(self.lastPoint, endPoint).normalized().adjusted(-rad, -rad, +rad, +rad))
		self.lastPoint = QPoint(endPoint)

	def resizeImage(self, image, newSize):
		if image.size() == newSize:
			return

		newImage = QImage(newSize, QImage.Format_RGBA8888)
		newImage.fill(Qt.transparent)
		painter = QPainter(newImage)
		painter.drawImage(QPoint(0, 0), image)
		self.image = newImage

if __name__ == '__main__':

	import sys

	app = QApplication(sys.argv)
	window = ScribbleArea()
	window.show()
	sys.exit(app.exec_())
