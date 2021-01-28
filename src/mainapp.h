#include <QWidget>
#include <QtGlobal>
#include <QPen>
#include "undoque.h"

class MainApp:public QWidget {
	Q_OBJECT
public:
	explicit MainApp(int w, int h);
protected:
	void keyPressEvent(QKeyEvent *event) override;
	void paintEvent(QPaintEvent *event) override;
	void tabletEvent(QTabletEvent *event) override;
	void resizeEvent(QResizeEvent *event) override;
private:
	void clearImage();
	void setpen(bool is_eraser);
	void tool_pen(QTabletEvent *event);
	void tool_paintbrush(QTabletEvent *event);
	bool drawing;
	QPointF lastpos;
	qreal lastpressure;
	QImage image;
	QPen pen;
	UndoQue undoque;

	// 'a': paintbrush
	// 'A': pen/eraser
	char tool_mode = 'A';
};
