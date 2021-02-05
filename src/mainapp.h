#pragma once

#include <QMainWindow>
#include <QtGlobal>
#include <QColor>
#include <QImage>
#include <QPen>
#include <QRectF>
#include "undoque.h"
#include "resource_manager.h"

class MainApp:public QMainWindow {
	Q_OBJECT
public:
	explicit MainApp(int w, int h);
	void set_main_color(QColor new_color);

	ResourceManager resource_manager;
protected:
	void keyPressEvent(QKeyEvent *event) override;
	void paintEvent(QPaintEvent *event) override;
	void tabletEvent(QTabletEvent *event) override;
private:
	void clearImage();
	void setpen(bool is_eraser);
	QPointF transform_view(QPointF p0);
	QPointF transform_view_rev(QPointF p0);
	void pan_image(QPointF dr);
	void tool_pen(QTabletEvent *event);
	void tool_paintbrush(QTabletEvent *event);
	void proc_resize();
	int button_state; // 0 nothing, 1 drawing, 2 panning
	QPointF lastpos;
	qreal lastpressure;
	QRectF view;
	QImage image;
	QPen pen;
	QColor main_color;
	UndoQue undoque;
	QWidget* activeWidget;
	int currentWidgetType;

	// 'a': paintbrush
	// 'A': pen/eraser
	char tool_mode;
};
