#include <QPalette>
#include <QDebug>
#include <QPainter>
#include <QPen>
#include <QRect>
#include <QTabletEvent>
#include <QPaintEvent>
#include "colorsel.h"

ColorSel::ColorSel(MainApp* parent) {
	setAttribute(Qt::WA_StaticContents, true);
	auto pw = parent->width();
	auto ph = parent->height();
	int s = 256;
	move(pw / 2 - s / 2, ph / 2 - s / 2);
	resize(s, s);
	QPalette pal = palette();
	pal.setColor(QPalette::Window, Qt::red);
	setAutoFillBackground(true);
	setPalette(pal);
	setParent(parent);
	mainapp = parent;

	auto bkgnd = mainapp->resource_manager.colorsel1.scaled(this->size(), Qt::IgnoreAspectRatio);
	QPalette palette;
	palette.setBrush(QPalette::Window, bkgnd);
	this->setPalette(palette);
}

void ColorSel::paintEvent(QPaintEvent *event) {
	auto painter = QPainter(this);
	painter.drawLine(QPointF(pf.x(), 0), QPointF(pf.x(), height()));
	painter.drawLine(QPointF(0, pf.y()), QPointF(width(), pf.y()));
}

void ColorSel::tabletEvent(QTabletEvent *event) {
	auto r1 = QRect(QPoint(pf.x() - 1, 0), QPoint(pf.x() + 1, height()));
	auto r2 = QRect(QPoint(0, pf.y() - 1), QPoint(width(), pf.y() + 1));
	update(r1);
	update(r2);
	pf = event->posF();
	if(pf.x() < 0) {pf.setX(0);} 
	if(pf.x() >= width()) {pf.setX(width() - 0.1);} 
	if(pf.y() < 0) {pf.setY(0);} 
	if(pf.y() >= height()) {pf.setY(height() - 0.1);} 
	qDebug() << pf;
	mainapp->set_main_color(QColor::fromHsv(
		pf.x() / width() * 360,
		pf.y() / height() * 256,
		255
	));
}
