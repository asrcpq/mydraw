#include <Qt>
#include <QDebug>
#include <QKeyEvent>
#include <QTabletEvent>
#include <QPaintEvent>
#include <QPoint>
#include <QRect>
#include <QRectF>
#include <QResizeEvent>
#include <QPainter>
#include <QPen>
#include <QImage>
#include "mainapp.h"

#include <algorithm>
using std::max;

MainApp::MainApp(int w, int h) {
	resize(w, h);
	setWindowTitle("mydraw");
	setAttribute(Qt::WA_StaticContents, true);
	setAttribute(Qt::WA_TranslucentBackground, true);
	update();
	drawing = false;
}

void MainApp::keyPressEvent(QKeyEvent *event) {
	if(event->key() == Qt::Key_Q) {
		close();
	} else if(event->key() == Qt::Key_C) {
		clearImage();
	}
}

void MainApp::tabletEvent(QTabletEvent *event) {
	switch (event->type()) {
		case QEvent::TabletPress:
			if(!drawing) {
				drawing = true;
				lastpos = event->posF();
				lastpressure = event->pressure();
			}
			break;
		case QEvent::TabletMove:
			if(drawing) {
				//qDebug() << *event;
				QPainter painter(&image);
				QPen pen;
				pen.setStyle(Qt::SolidLine);
				pen.setWidth(2);
				pen.setBrush(Qt::green);
				pen.setCapStyle(Qt::RoundCap);
				pen.setJoinStyle(Qt::RoundJoin);
				painter.setPen(pen);
				painter.drawLine(lastpos, event->posF());
				int r = 4;
				QRect update_rect = QRect(
					QPoint(lastpos.x(), lastpos.y()),
					event->pos()
				).normalized().adjusted(-r, -r, +r, +r);
				// QRect update_rect = QRect(QPoint(0,0), QPoint(640, 480));
				lastpos = event->posF();
				lastpressure = event->pressure();
				update(update_rect);
			}
			break;
		case QEvent::TabletRelease:
			if (drawing && event->buttons() == Qt::NoButton)
				drawing = false;
			break;
		default:
			break;
	}
	event->accept();
}

void MainApp::paintEvent(QPaintEvent *event) {
	QPainter painter(this);
	QRect dirtyRect = event->rect();
	painter.drawImage(dirtyRect, this->image, dirtyRect);
}

void MainApp::resizeEvent(QResizeEvent *event) {
	if(width() > image.width() || height() > image.height()) {
		int newWidth = max(width() + 128, image.width());
		int newHeight = max(height() + 128, image.height());
		QImage newImage(newWidth, newHeight, QImage::Format_RGBA8888);
		newImage.fill(Qt::transparent);
		QPainter painter(&newImage);
		painter.drawImage(QPoint(0, 0), image);
		image = std::move(newImage);
		//update();
	}
}

void MainApp::clearImage() {
	QImage newImage(width(), height(), QImage::Format_RGBA8888);
	newImage.fill(Qt::transparent);
	image = std::move(newImage);
	update();
}
