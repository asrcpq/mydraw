#include <Qt>
#include <QApplication>
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

//for image save
#include <ctime>
#include <cstdlib>
#include <sstream>

#include <algorithm>
using std::max;

MainApp::MainApp(int w, int h) {
	resize(w, h);
	setWindowTitle("mydraw");
	setAttribute(Qt::WA_StaticContents, true);
	setAttribute(Qt::WA_TranslucentBackground, true);
	drawing = false;
	setpen(false);
}

void MainApp::keyPressEvent(QKeyEvent *event) {
	auto modifiers = QApplication::keyboardModifiers();
	if(modifiers == Qt::ControlModifier) {
		if(event->key() == Qt::Key_C) {
			close();
		}
		if(event->key() == Qt::Key_S) {
			const char* s = getenv("XDG_DATA_HOME");
			std::stringstream out;
			out << s << "/mydraw/image_save/" << time(NULL) << ".png";
			image.save(QString::fromStdString(out.str()));
		}
	} else if(event->key() == Qt::Key_C) {
		clearImage();
	} else if(event->key() == Qt::Key_E) {
		setpen(true);
	} else if(event->key() == Qt::Key_A) {
		setpen(false);
	}
}

void MainApp::setpen(bool is_eraser) {
	if(is_eraser) {
		pen.setStyle(Qt::SolidLine);
		pen.setWidth(30);
		pen.setBrush(Qt::transparent);
		pen.setCapStyle(Qt::RoundCap);
		pen.setJoinStyle(Qt::RoundJoin);
	} else {
		pen.setStyle(Qt::SolidLine);
		pen.setWidth(3);
		pen.setBrush(Qt::green);
		pen.setCapStyle(Qt::RoundCap);
		pen.setJoinStyle(Qt::RoundJoin);
	}
}

void MainApp::tabletEvent(QTabletEvent *event) {
	switch (event->type()) {
		case QEvent::TabletPress:
			if(!drawing && event->buttons() == Qt::LeftButton) {
				drawing = true;
				lastpos = event->posF();
				lastpressure = event->pressure();
			}
			break;
		case QEvent::TabletMove:
			if(drawing) {
				QPainter painter(&image);
				painter.setPen(pen);
				painter.setCompositionMode(QPainter::CompositionMode_Source);
				painter.setRenderHint(QPainter::Antialiasing, true);
				painter.drawLine(lastpos, event->posF());
				int r = pen.width() + 1;
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
