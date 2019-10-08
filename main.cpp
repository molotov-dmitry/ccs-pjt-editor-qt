#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    for (int i = 1; i < argc; ++i)
    {
        w.openProject(QString::fromStdString(argv[i]));
    }

    return a.exec();
}
