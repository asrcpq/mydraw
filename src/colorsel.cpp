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
	mainapp->set_main_color(QColor::fromHsv(
		pf.x() / width() * 256,
		pf.y() / height() * 256,
		255
	));
}
