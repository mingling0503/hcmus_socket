#ifndef WELCOMESCREEN_H
#define WELCOMESCREEN_H

#include <QWidget>
#include <QTimer>

class WelcomeScreen : public QWidget {
    Q_OBJECT

public:
    explicit WelcomeScreen(QWidget* parent = nullptr);

signals:
    void spacePressed();      // Tín hiệu phát khi nhấn phím Space
    void welcomeFinished();   // Tín hiệu phát khi hoàn tất màn hình Welcome

protected:
    void keyPressEvent(QKeyEvent* event) override; // Ghi đè sự kiện nhấn phím

private:
    QTimer* timer; // Không cần nếu chỉ sử dụng Space
};

#endif // WELCOMESCREEN_H
