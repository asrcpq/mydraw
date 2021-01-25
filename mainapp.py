from PyQt5.QtCore import QDir, QPoint, QPointF, QRect, QSize, Qt
from PyQt5.QtGui import QImage, QPainter, QBrush, qRgba, QTabletEvent
from PyQt5.QtWidgets import QApplication, QWidget

from collections import deque
class MainApp(QWidget):
	def __init__(self, wsize):
		super(MainApp, self).__init__()

		self.setAttribute(Qt.WA_StaticContents)
		self.scribbling = False
		self.image = QImage()
		self.resize(wsize[0], wsize[1])

		self.setWindowTitle("mydraw")
		self.setAttribute(Qt.WA_TranslucentBackground)

		with open('./utils/pb.png', 'rb') as f:
			content = f.read()
		self.pbimage = QImage()
		self.pbimage.loadFromData(content)
		painter = QPainter(self.pbimage)
		painter.setCompositionMode(QPainter.CompositionMode_SourceIn);
		painter.fillRect(self.pbimage.rect(), Qt.red)

		self.undo_stack0 = []
		self.undo_stack1 = deque()
		self.undo_stack1_maxlen = 100

	def clearImage(self):
		self.image.fill(Qt.transparent)
		self.update()

	def keyPressEvent(self, event):
		key = event.key()
		modifiers = QApplication.keyboardModifiers()
		if modifiers == Qt.ControlModifier:
			if key == Qt.Key_C:
				self.close()
		if key == Qt.Key_C:
			self.clearImage()
		if key == Qt.Key_Z:
			self.undo()

	def undo_push0(self, pos, img):
		if len(self.undo_stack1) >= self.undo_stack1_maxlen:
			self.undo_stack1.popleft()
		self.undo_stack0.append((pos, img))

	def undo(self):
		if len(self.undo_stack1) == 0:
			print("No more undo history")
			return
		painter = QPainter(self.image)
		painter.setCompositionMode(QPainter.CompositionMode_Source);
		for pos, img in reversed(self.undo_stack1.pop()):
			painter.drawImage(pos, img)
			self.update(QRect(
				pos,
				pos + QPoint(img.width(), img.height())
			))

	def drawLineWithImage(self, endPoint):
		bsize = 3
		radius = bsize / 2
		undo_rad = radius + 1
		rect = QRect(self.lastPoint[0], endPoint[0])\
			.normalized()\
			.adjusted(-undo_rad, -undo_rad, +undo_rad, +undo_rad)
		painter = QPainter(self.image)
		oldimg = self.image.copy(rect)
		self.undo_push0(rect.topLeft(), oldimg)

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
		r = QPointF(self.lastPoint[0] - QPoint(radius, radius))
		p = self.lastPoint[1]
		while dist > 0:
			painter.setOpacity(p);
			#print(r)
			painter.drawImage(r, self.pbimage.scaled(bsize, bsize))
			r += dr
			p += dp
			dist -= spacing
		self.update(rect)
		self.lastPoint = endPoint

	def tabletEvent(self, tabletEvent):
		self.pos = tabletEvent.pos()
		self.pressure = tabletEvent.pressure()

		if tabletEvent.type() == QTabletEvent.TabletPress:
			self.pen_is_down = True
		elif tabletEvent.type() == QTabletEvent.TabletMove:
			self.pen_is_down = True
		elif tabletEvent.type() == QTabletEvent.TabletRelease:
			self.pen_is_down = False
		if self.pen_is_down:
			if not self.scribbling:
				self.lastPoint = (self.pos, self.pressure)
				self.scribbling = True
			else:
				self.drawLineWithImage((self.pos, self.pressure))
		else:
			if self.scribbling:
				self.scribbling = False
				self.undo_stack1.append(self.undo_stack0)
				self.undo_stack0 = []
			self.drawLineWithImage((self.pos, self.pressure))
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

		super(MainApp, self).resizeEvent(event)

	def resizeImage(self, image, newSize):
		if image.size() == newSize:
			return

		newImage = QImage(newSize, QImage.Format_RGBA8888)
		newImage.fill(Qt.transparent)
		painter = QPainter(newImage)
		painter.drawImage(QPoint(0, 0), image)
		self.image = newImage
