#include <Qt>
#include <QApplication>
#include <QDebug>
#include <QKeyEvent>
#include <QTabletEvent>
#include <QPaintEvent>
#include <QPoint>
#include <QRect>
#include <QRectF>
#include <QLineF>
#include <QPainter>
#include <QPen>
#include <QImage>
#include "mainapp.h"
#include "colorsel.h"

//for image save
#include <ctime>
#include <cstdlib>
#include <sstream>

#include <algorithm>
using std::max;

MainApp::MainApp(int w, int h) {
	main_color = Qt::green;
	activeWidget = nullptr;
	currentWidgetType = 0;
	resize(w, h);
	view = QRectF(0, 0, w, h);
	proc_resize();
	setWindowTitle("mydraw");
	setAttribute(Qt::WA_StaticContents, true);
	setAttribute(Qt::WA_TranslucentBackground, true);
	button_state = 0;
	setpen(false);
	tool_mode = 'a';
}

void MainApp::set_main_color(QColor new_color) {
	main_color = std::move(new_color);
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
	} else if(modifiers == Qt::ShiftModifier) {
		if(event->key() == Qt::Key_A) {
			tool_mode = 'A';
			setpen(false);
		} else if(event->key() == Qt::Key_E) {
			tool_mode = 'A';
			setpen(true);
		}
	} else if(event->key() == Qt::Key_C) {
		clearImage();
	} else if(event->key() == Qt::Key_Q) {
		if(activeWidget == nullptr || currentWidgetType != 1) {
			delete activeWidget;
			activeWidget = new ColorSel(this);
			currentWidgetType = 1;
			activeWidget->show();
		} else {
			delete activeWidget;
			activeWidget = nullptr;
			currentWidgetType = 0;
		}
	} else if(event->key() == Qt::Key_R) {
		proc_resize();
		update();
	} else if(event->key() == Qt::Key_A) {
		tool_mode = 'a';
	} else if(event->key() == Qt::Key_Z) {
		undoque.undo(image);
		// TODO: update rect?
		update();
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

void MainApp::tool_pen(QTabletEvent *event) {
	int r = pen.width() + 1;
	auto newpos = transform_view_rev(event->posF());
	auto lastpos_canvas = transform_view_rev(lastpos);
	QRect update_rect = QRect(
		QPoint(lastpos.x(), lastpos.y()),
		event->pos()
	).normalized().adjusted(-r, -r, +r, +r);
	undoque.push1(update_rect.topLeft(), image.copy(update_rect));

	QPainter painter(&image);
	painter.setPen(pen);
	painter.setCompositionMode(QPainter::CompositionMode_Source);
	painter.setRenderHint(QPainter::Antialiasing, true);
	// drawline after push1
	painter.drawLine(lastpos_canvas, newpos);
	update(update_rect);
}

void MainApp::tool_paintbrush(QTabletEvent *event) {
	auto newpos = transform_view_rev(event->posF());
	auto newpres = event->pressure();
	auto lastpos_canvas = transform_view_rev(lastpos);
	auto rk = 2;
	auto rmax = max(newpres, lastpressure) * rk + 3;
	QRect update_rect = QRect(
		QPoint(lastpos.x(), lastpos.y()),
		event->pos()
	).normalized().adjusted(-rmax, -rmax, +rmax, +rmax);
	undoque.push1(update_rect.topLeft(), image.copy(update_rect));

	auto dist = QLineF(newpos, lastpos_canvas).length();
	auto rnorm = (newpos - lastpos_canvas) / dist;
	auto x = rnorm.x();
	auto y = rnorm.y();
	auto s1 = lastpressure * rk;
	auto s2 = newpres * rk;
	// 1<| lu ru rd ld
	QPointF parray[4] = {
		s1 * QPointF(-y, x) + lastpos_canvas,
		s2 * QPointF(-y, x) + newpos,
		s2 * QPointF(y, -x) + newpos,
		s1 * QPointF(y, -x) + lastpos_canvas
	};
	auto painter = QPainter(&image);
	painter.setBrush(main_color);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setPen(Qt::NoPen);
	painter.drawConvexPolygon(parray, 4);
	auto sr = s2;
	painter.drawEllipse(newpos, sr, sr);
	update(update_rect);
}

void MainApp::tabletEvent(QTabletEvent *event) {
	if(activeWidget != nullptr) {return;}
	switch (event->type()) {
		case QEvent::TabletPress:
			if(button_state == 0) {
				if (event->buttons() == Qt::LeftButton) {
					button_state = 1;
				} else if (event->buttons() == Qt::MiddleButton) {
					button_state = 2;
				}
			}
			lastpos = event->posF();
			lastpressure = event->pressure();
			break;
		case QEvent::TabletMove:
			if(button_state == 1) {
				if(tool_mode == 'A') {
					tool_pen(event);
				} else if(tool_mode == 'a') {
					tool_paintbrush(event);
				}
			} else if(button_state == 2) {
				pan_image(lastpos - event->posF());
			}
			lastpos = event->posF();
			lastpressure = event->pressure();
			break;
		case QEvent::TabletRelease:
			if (button_state != 0 && event->buttons() == Qt::NoButton)
				button_state = 0;
				undoque.finish();
			break;
		default:
			break;
	}
	event->accept();
}

void MainApp::pan_image(QPointF dr) {
	view.adjust(dr.x(), dr.y(), dr.x(), dr.y());
	update();
}

// absolute point -> canvas point
inline QPointF MainApp::transform_view(QPointF p0) {
	return QPointF(
		(p0.x() - view.left()) / width() * view.width(),
		(p0.y() - view.top()) / height() * view.height()
	);
}

// canvas point -> absolute point
inline QPointF MainApp::transform_view_rev(QPointF p0) {
	return QPointF(
		p0.x() * width() / view.width() + view.left(),
		p0.y() * height() / view.height() + view.top()
	);
}

void MainApp::paintEvent(QPaintEvent *event) {
	QPainter painter(this);
	QRectF dirtyRect = event->rect();
	// transform to view
	auto new_lu = transform_view_rev(dirtyRect.topLeft());
	auto new_rd = transform_view_rev(dirtyRect.bottomRight());
	auto transformed_dirty = QRectF(new_lu, new_rd);
	// qDebug() << transformed_dirty << dirtyRect;
	painter.drawImage(dirtyRect, this->image, transformed_dirty);
}

void MainApp::proc_resize() {
	qDebug() << width() << height();
	if(width() > image.width() || height() > image.height()) {
		int newWidth = max(width() + 128, image.width());
		int newHeight = max(height() + 128, image.height());
		QImage newImage(newWidth, newHeight, QImage::Format_RGBA8888);
		newImage.fill(Qt::transparent);
		QPainter painter(&newImage);
		painter.drawImage(QPoint(0, 0), image);
		image = std::move(newImage);
	}
}

void MainApp::clearImage() {
	QImage newImage(width(), height(), QImage::Format_RGBA8888);
	newImage.fill(Qt::transparent);
	image = std::move(newImage);
	update();
}
