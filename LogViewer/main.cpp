#include "MainWindowUI.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindowUI w;
	w.show();
	return a.exec();
}
