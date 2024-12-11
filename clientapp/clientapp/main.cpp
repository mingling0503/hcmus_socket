#include <QApplication>
#include "mainwindow.h"
#include <QLabel>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    MainWindow mainWindow;
    mainWindow.show();
    //QIcon appIcon("logo.png");
    //mainWindow.setWindowIcon(appIcon);
    return app.exec();
}