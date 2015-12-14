#include <QApplication>

#include "vld.h"
#include "CustomDialog.h"
#include "LogoinWidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    LogoinWidget w;
    w.show();
    w.showMaximized();

    return a.exec();
}
