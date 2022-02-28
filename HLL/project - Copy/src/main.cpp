#include "MainWindow.h"
#include "version.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationVersion(VER_PRODUCTVERSION_STR);

    MainWindow w(app.arguments());
    w.show();
    return app.exec();
}
