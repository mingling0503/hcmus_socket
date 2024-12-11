#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QStringList>
#include <QTimer>
#include "clientservice.h"
#include "registerdialog.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void updateLog(const QString& message);
    void flushLogBuffer();
    void onRegisterMailClicked();
    void onStartServiceClicked();
    void onStopServiceClicked();

private:
    QPushButton* registerMailButton;
    QPushButton* startServiceButton;
    QPushButton* stopServiceButton;
    QTextEdit* logTextEdit;

    QStringList logBuffer;
    QTimer* logUpdateTimer;

    ClientService* clientService;
};

#endif // MAINWINDOW_H