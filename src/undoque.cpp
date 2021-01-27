#include <QPainter>
#include <QRect>
#include <QDebug>

#include "undoque.h"

void UndoQue::push1(QPoint pos, QImage img) {
	if(finish_flag){
		data.emplace_back();
		finish_flag = false;
	}
	data.back().emplace_back(std::make_pair(pos, img));
}

void UndoQue::finish() {
	finish_flag = true;
}

void UndoQue::push2(QPoint pos, QImage img) {
	if(finish_flag){
		data.emplace_back();
	}
	data.end()->emplace_back(std::make_pair(pos, img));
	finish_flag = true;
}

int UndoQue::undo(QImage &image) {
	// if drawing, do nothing
	if(!finish_flag) {
		return 1;
	}
	if(data.empty()) {
		return 1;
	}
	auto painter = QPainter(&image);
	painter.setCompositionMode(QPainter::CompositionMode_Source);
	auto back_vec = data.back();
	for (auto i = back_vec.rbegin(); i != back_vec.rend(); ++i) { 
		painter.drawImage(i->first, i->second);
	}
	data.pop_back();
	return 0;
}
