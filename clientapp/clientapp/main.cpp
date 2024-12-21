#include <QApplication>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include "MainWindow.h"
#include "WelcomeScreen.h"
#include "FontManager.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    FontManager::getInstance().initialize();

    WelcomeScreen welcomeWindow;
    welcomeWindow.setWindowTitle("Welcome");
    welcomeWindow.resize(900, 600);
    welcomeWindow.show();

    MainWindow mainWindow;
    mainWindow.setWindowTitle("PC Remote Control Service");
    mainWindow.resize(900, 600);

    QObject::connect(&welcomeWindow, &WelcomeScreen::spacePressed, [&]() {
        QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect(&welcomeWindow);
        welcomeWindow.setGraphicsEffect(opacityEffect);

        QPropertyAnimation* fadeOut = new QPropertyAnimation(opacityEffect, "opacity");
        fadeOut->setDuration(1000);
        fadeOut->setStartValue(1.0);
        fadeOut->setEndValue(0.0);

        QObject::connect(fadeOut, &QPropertyAnimation::finished, [&]() {
            welcomeWindow.close();
            mainWindow.show();
            });

        fadeOut->start();
        });

    return app.exec();
}