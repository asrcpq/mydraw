#include <QApplication>
#include <QGuiApplication>
#include <QScreen>
#include <QRect>
#include "mainapp.h"

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	QSize screen_rect = QGuiApplication::screens()[0]->size();
	MainApp mainapp(screen_rect.width(), screen_rect.height());
	mainapp.show();

	return a.exec(); // .exec starts QApplication and related GUI, this line starts 'event loop'	
}
