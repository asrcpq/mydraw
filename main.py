from PyQt5.QtCore import QDir, QPoint, QPointF, QRect, QSize, Qt
from PyQt5.QtGui import (
	QImage, QImageWriter, QPainter, QBrush, qRgba,
	QTabletEvent
)
from PyQt5.QtWidgets import (
	QAction, QApplication, QColorDialog, QFileDialog, QInputDialog,
	QMainWindow, QMenu, QMessageBox, QWidget
)

import numpy as np

class ScribbleArea(QWidget):
	def __init__(self, parent=None):
		super(ScribbleArea, self).__init__(parent)

		self.setAttribute(Qt.WA_StaticContents)
		self.modified = False
		self.scribbling = False
		self.myPenWidth = 3
		self.myPenColor = Qt.red
		self.image = QImage()

		frame_rect = app.desktop().frameGeometry()
		width, height = frame_rect.width(), frame_rect.height()
		self.resize(width, height)

		self.setWindowTitle("mydraw")
		self.setAttribute(Qt.WA_TranslucentBackground)
		self.setWindowOpacity(0.5)

	def setPenColor(self, newColor):
		self.myPenColor = newColor

	def setPenWidth(self, newWidth):
		self.myPenWidth = newWidth

	def clearImage(self):
		self.image.fill(Qt.transparent)
		self.modified = True
		self.update()

	def keyPressEvent(self, event):
		key = event.key()
		if key == Qt.Key_C:
			self.clearImage()
		if key == Qt.Key_Q:
			self.close()

	def drawLineWithImage(self, endPoint):
		bsize = 3
		painter = QPainter(self.image)
		rad = bsize / 2
		spacing = 1
		relpos = endPoint[0] - self.lastPoint[0]
		dist = (relpos.x() ** 2 + relpos.y() ** 2) ** 0.5
		if dist == 0:
			return
		inv_step = spacing / dist
		dr = QPointF(relpos) * inv_step
		max(endPoint[1], self.lastPoint[1]) / 2 * bsize
		dp = (endPoint[1] - self.lastPoint[1]) * inv_step
		# move brush image to center
		r = QPointF(self.lastPoint[0] - QPoint(rad, rad))
		p = self.lastPoint[1]
		while dist > 0:
			painter.setOpacity(p);
			painter.drawImage(r, pbimage.scaled(bsize, bsize))
			r += dr
			p += dp
			dist -= spacing
		self.modified = True

		self.update(
			QRect(self.lastPoint[0],
				  endPoint[0]).normalized().adjusted(-rad, -rad, +rad, +rad))
		# self.update()
		self.lastPoint = endPoint

	def tabletEvent(self, tabletEvent):
		self.pos = tabletEvent.pos()
		# Mapping
		p = tabletEvent.pressure()
		g = 0.2
		k = (0.5 - 0.25 * g) / 0.25
		if p <= 0.25:
			self.pressure = g * p
		elif p > 0.75:
			self.pressure = g * (p - 1) + 1
		else:
			self.pressure = k * (p - 0.5) + 0.5

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
				self.lastPoint = (self.pos, self.pressure)
				self.scribbling = True
			else:
				self.drawLineWithImage((self.pos, self.pressure))
		else:
			self.drawLineWithImage((self.pos, self.pressure))
			self.scribbling = False
		tabletEvent.accept()

	def paintEvent(self, event):
		painter = QPainter(self)
		dirtyRect = event.rect()
		painter.drawImage(dirtyRect, self.image, dirtyRect)

	def resizeEvent(self, event):
		if self.width() > self.image.width() or self.height(
		) > self.image.height():
			newWidth = max(self.width() + 128, self.image.width())
			newHeight = max(self.height() + 128, self.image.height())
			self.resizeImage(self.image, QSize(newWidth, newHeight))
			self.update()

		super(ScribbleArea, self).resizeEvent(event)

	def resizeImage(self, image, newSize):
		if image.size() == newSize:
			return

		newImage = QImage(newSize, QImage.Format_RGBA8888)
		newImage.fill(Qt.transparent)
		painter = QPainter(newImage)
		painter.drawImage(QPoint(0, 0), image)
		self.image = newImage

pbimage = QImage()
if __name__ == '__main__':
	with open('pb.png', 'rb') as f:
		content = f.read()
	pbimage.loadFromData(content)

	import sys

	app = QApplication(sys.argv)
	window = ScribbleArea()
	window.show()
	sys.exit(app.exec_())
