#include "WelcomeScreen.h"
#include "CurveWidget.h"
#include <QVBoxLayout>
#include <QPalette>
#include <QKeyEvent>

WelcomeScreen::WelcomeScreen(QWidget* parent)
    : QWidget(parent) {
    setWindowTitle("Welcome");
    resize(800, 600);

    // Đặt màu nền
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(255, 248, 231));
    this->setAutoFillBackground(true);
    this->setPalette(palette);

    // Thêm CurveWidget vào layout
    CurveWidget* curveWidget = new CurveWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(curveWidget);
    setLayout(layout);
}

void WelcomeScreen::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Space) {
        emit spacePressed(); // Phát tín hiệu khi nhấn Space
    }
    else {
        QWidget::keyPressEvent(event); // Xử lý phím khác
    }
}
