/*#######################################################################
        > File Name: main.cpp
        # Author:       WIND
        # mail:         fengleyl@163.com
        > Created Time: 2015��05��23�� ������ 11ʱ35��51��
########################################################################*/


#include "mainwidget/mainwindow.h"

#include <QFile>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if (!connectDatabase())
    {
        return 1;
    }

    QFile file(":/qss/default");
    file.open(QFile::ReadOnly);
    qApp->setStyleSheet(file.readAll());
    file.close();

    MainWindow w;
    w.show();

    return a.exec();
}
