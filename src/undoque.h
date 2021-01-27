#include <deque>
#include <vector>
#include <tuple>
#include <QPoint>
#include <QImage>

typedef std::vector<std::pair<QPoint, QImage>> UndoStep;

class UndoQue {
public:
	void push1(QPoint pos, QImage img);
	void finish();
	void push2(QPoint pos, QImage img);
	int undo(QImage &image);
private:
	// deque for possible max_size
	std::deque<UndoStep> data;
	bool finish_flag = true;
};
