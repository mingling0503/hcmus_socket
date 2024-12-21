#ifndef CURVEWIDGET_H
#define CURVEWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QElapsedTimer>
#include <QPainterPath>

class CurveWidget : public QWidget {
    Q_OBJECT

public:
    // Constructor
    explicit CurveWidget(QWidget* parent = nullptr);

protected:
    // Hàm xử lý sự kiện vẽ
    void paintEvent(QPaintEvent* event) override;

private:
    // Hàm cập nhật đường cong
    void updateCurve();

    // Hàm vẽ hiệu ứng dòng chữ
    void drawTextWithEffect(QPainter* painter);


    QTimer* timer;                // Bộ hẹn giờ để cập nhật giao diện
    QElapsedTimer elapsedTimer;   // Bộ đếm thời gian để tính hiệu ứng động
    double t;                     // Tham số góc để vẽ đường cong
    QColor currentColor;          // Màu sắc động của đường cong
    QPainterPath fullPath;        // Đường cong đầy đủ đã vẽ
};

#endif // CURVEWIDGET_H
