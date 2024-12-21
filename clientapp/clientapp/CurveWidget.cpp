#include "CurveWidget.h"
#include "FontManager.h"
#include <QPainter>
#include <QtMath>
#include <QLinearGradient>

CurveWidget::CurveWidget(QWidget* parent)
    : QWidget(parent), t(0.0), currentColor(Qt::blue) {
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &CurveWidget::updateCurve);
    timer->start(30); 

    elapsedTimer.start();
}

void CurveWidget::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.translate(width() / 2, height() / 2);

    QLinearGradient gradient(QPointF(-200, -200), QPointF(200, 200)); 
    gradient.setColorAt(0.0, currentColor);
    gradient.setColorAt(1.0, currentColor.darker(150));
    painter.setBrush(Qt::NoBrush);

    QPen pen(QBrush(gradient), 2);
    painter.setPen(pen);

    painter.drawPath(fullPath);

    drawTextWithEffect(&painter);
}

void CurveWidget::drawTextWithEffect(QPainter* painter) {
    int elapsed = elapsedTimer.elapsed();

    int red = 0;
    int green = 0;
    int blue = 118;
    QColor textColor(red, green, blue);

    QFont font(FontManager::getInstance().getLabelFont());
    QFont font2(FontManager::getInstance().getLogFont());
    QFont font3(FontManager::getInstance().getButtonFont());
    painter->setFont(font);
    painter->setPen(QPen(textColor));

    QString text1 = "PC REMOTE CONTROL via EMAIL";
    QString text2 = "Welcome!";

    QString text3 = "Take a seat";
    QString text4 = "and control ...";
    QString text5 = "Press 'space' to continue";

    int yPosition = -height() / 2 + 50 + (qSin(elapsed * 0.002) * 10); 
    painter->drawText(-painter->fontMetrics().horizontalAdvance(text1) / 2, yPosition, text1);
    int yPosition2 = yPosition + 40;
    painter->drawText(-painter->fontMetrics().horizontalAdvance(text2) / 2, yPosition2, text2);

    painter->setFont(font3);
    int yPosition3 = -height() / 2 + 287;
    painter->drawText(-painter->fontMetrics().horizontalAdvance(text3) / 2, yPosition3, text3);
    int yPosition4 = yPosition3 + 30;
    painter->drawText(-painter->fontMetrics().horizontalAdvance(text4) / 2, yPosition4, text4);

    int yPosition5 = yPosition4 + 230;
    painter->setFont(font2);
    painter->drawText(-painter->fontMetrics().horizontalAdvance(text5) / 2, yPosition5, text5);
}

void CurveWidget::updateCurve() {
    QPainterPath newSegment;

    bool firstPoint = true;
    for (double angle = t; angle <= t + 0.1; angle += 0.005) { 
        double x = cos(angle) + cos(8 * angle) / 8; 
        double y = sin(angle) - sin(8 * angle) / 8;
        QPointF point(x * 150, y * 150); 

        if (firstPoint) {
            newSegment.moveTo(point);
            firstPoint = false;
        }
        else {
            newSegment.lineTo(point);
        }
    }

    fullPath.addPath(newSegment);

    
    t += 0.1;
    if (t > 2 * M_PI) {
        t = 0;  

        
        QTransform transform;
        transform.rotate(10);
        fullPath = transform.map(fullPath);
    }

    int elapsed = elapsedTimer.elapsed();
    int red = (qSin(elapsed * 0.001) * 127) + 128;
    int green = (qSin(elapsed * 0.002 + M_PI / 2) * 127) + 128;
    int blue = (qSin(elapsed * 0.003 + M_PI) * 127) + 128;
    currentColor = QColor(red, green, blue);

    update(); 
}
