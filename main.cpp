#include <iostream>
#include <memory>
#include <QApplication>
#include <mainwindow.h>

int main( int argc, char *argv[] )
{
	QApplication a( argc, argv );
	auto base = std::make_shared<Base>();
	MainWindow w( base );
	w.show();

	int returnCode = a.exec();

	return returnCode;
}
