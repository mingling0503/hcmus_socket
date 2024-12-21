#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>
#include <QTimer>
#include <QStringList>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include "ClientService.h" // Ensure this header exists and is implemented

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void showLoginDialog();
    void showRegisterDialog();
    void showAboutDialog();
    void showHelpDialog();
    void onLogoutClicked();
    void onStartServiceClicked();
    void onStopServiceClicked();
    //void onRegisterMailClicked();

private:
    void setupUI();
    void updateUIBasedOnLoginStatus();
    void updateLog(const QString& message);
    void flushLogBuffer();

    // Login and account-related fields
    bool isLoggedIn;
    QString currentUserEmail;

    // UI Elements
    QPushButton* loginButton;
    QPushButton* registerButton;
    QPushButton* aboutButton;
    QPushButton* helpButton;
    QPushButton* startServiceButton;
    QPushButton* stopServiceButton;
    QPushButton* logoutButton;
    QLabel* userEmailLabel;
    QTextEdit* logTextEdit;

    // Timer for log updates
    QTimer* logUpdateTimer;

    // Log management
    QStringList logBuffer;

    // Client service for remote control
    ClientService* clientService;

//private:
//    void initializeFonts();
//    QFont defaultButtonFont;    // Cooper Medium
//    QFont defaultLabelFont;     // Cooper Black
//    QFont defaultLogFont;       // Cooper Light
//
//public:
//    const QFont& getButtonFont() const { return defaultButtonFont; }
//    const QFont& getLabelFont() const { return defaultLabelFont; }
//    const QFont& getLogFont() const { return defaultLogFont; }
};


#endif // MAINWINDOW_H
