#pragma once
#include <QWidget>
#include <QPointF>
#include "mainapp.h"

class ColorSel:public QWidget {
	Q_OBJECT
public:
	explicit ColorSel(MainApp *parent);
protected:
	//void keyPressEvent(QKeyEvent *event) override;
	void tabletEvent(QTabletEvent *event) override;
	void paintEvent(QPaintEvent *event) override;
private:
	QPointF pf;
	MainApp* mainapp;
};
