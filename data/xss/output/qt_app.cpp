
#include <QtGui/QApplication>
#include "widget.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
	
    Widget window;
    window.show();
	
    return app.exec();
}
