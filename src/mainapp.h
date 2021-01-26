#include <QWidget>

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
	bool drawing;
	QPointF lastpos;
	float lastpressure;
	QImage image;
};
