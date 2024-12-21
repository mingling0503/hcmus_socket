#include "mainwindow.h"
#include "logindialog.h"
#include "FontManager.h"
#include "registerdialog.h"
#include <QMessageBox>
#include <QFile>
#include <QPalette>
#include <QScrollBar>
#include <QDateTime>
#include <QVBoxLayout>
#include <QTimer>
#include <QApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include "CurveWidget.h"
#include <QFontdatabase>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
    logUpdateTimer(new QTimer(this)),
    isLoggedIn(false),
    clientService(nullptr),
    loginButton(nullptr),
    registerButton(nullptr),
    aboutButton(nullptr),
    helpButton(nullptr),
    startServiceButton(nullptr),
    stopServiceButton(nullptr),
    logoutButton(nullptr),
    userEmailLabel(nullptr),
    logTextEdit(nullptr) {

    setupUI();
    updateUIBasedOnLoginStatus();
}

void MainWindow::setupUI() {

    QPalette palette;
    palette.setColor(QPalette::Window, QColor(255, 248, 231));
    setPalette(palette);
    setAutoFillBackground(true);


    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);
    layout->setContentsMargins(20, 40, 20, 40);
    layout->setSpacing(15);


    userEmailLabel = new QLabel("Welcome! Please login or register to access full features.", this);
    userEmailLabel->setFont(FontManager::getInstance().getLogFont());

    userEmailLabel->setStyleSheet("QLabel { color : black; }");
    layout->addWidget(userEmailLabel);
    // Create buttons
    loginButton = new QPushButton("Login", this);
    registerButton = new QPushButton("Register", this);
    aboutButton = new QPushButton("About", this);
    helpButton = new QPushButton("Help", this);

    // Style buttons
    QPushButton* buttons1[] = { loginButton, registerButton, aboutButton, helpButton };
    for (QPushButton* button : buttons1) {
        button->setFont(FontManager::getInstance().getButtonFont());
        button->setStyleSheet(
            "QPushButton {"
            "   background-color: #C2B280;"
            "   color: black;"
            "   padding: 10px;"
            "   border-radius: 5px;"
            "} "
            "QPushButton:hover {"
            "   background-color: #A67B5B;"
            "}"
        );
        layout->addWidget(button);
    }

    // Add additional controls for logged-in state


    startServiceButton = new QPushButton("Start Service", this);
    stopServiceButton = new QPushButton("Stop Service", this);
    logoutButton = new QPushButton("Logout", this);
    QPushButton* buttons2[] = { startServiceButton, stopServiceButton, logoutButton };
    for (QPushButton* button : buttons2) {
        button->setFont(FontManager::getInstance().getButtonFont());
        button->setStyleSheet(
            "QPushButton {"
            "   background-color: #C2B280;"
            "   color: black;"
            "   padding: 10px;"
            "   border-radius: 5px;"
            "} "
            "QPushButton:hover {"
            "   background-color: #A67B5B;"
            "}"
        );
        //layout->addWidget(button);
    }

    logTextEdit = new QTextEdit(this);
    logTextEdit->setReadOnly(true);
    logTextEdit->setFont(FontManager::getInstance().getLogFont());
    logTextEdit->setStyleSheet(
        "QTextEdit {"
        "   background-color: #EED9C4;"
        "   border-radius: 5px;"
        "   padding: 10px;"
        "}"
    );

    // Add widgets to layout
    layout->addWidget(userEmailLabel, 0, Qt::AlignCenter);
    layout->addWidget(startServiceButton);
    layout->addWidget(stopServiceButton);
    layout->addWidget(logoutButton);
    layout->addWidget(logTextEdit);


    // Hide logged-in UI elements initially
    startServiceButton->hide();
    stopServiceButton->hide();
    logoutButton->hide();
    logTextEdit->hide();

    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    // Connect signals and slots
    connect(loginButton, &QPushButton::clicked, this, &MainWindow::showLoginDialog);
    connect(registerButton, &QPushButton::clicked, this, &MainWindow::showRegisterDialog);
    connect(aboutButton, &QPushButton::clicked, this, &MainWindow::showAboutDialog);
    connect(helpButton, &QPushButton::clicked, this, &MainWindow::showHelpDialog);
    connect(startServiceButton, &QPushButton::clicked, this, &MainWindow::onStartServiceClicked);
    connect(stopServiceButton, &QPushButton::clicked, this, &MainWindow::onStopServiceClicked);
    connect(logoutButton, &QPushButton::clicked, this, &MainWindow::onLogoutClicked);

    connect(logUpdateTimer, &QTimer::timeout, this, &MainWindow::flushLogBuffer);
    logUpdateTimer->start(100);

    setWindowTitle("PC Remote Control Service");
    resize(600, 600);
}

void MainWindow::updateUIBasedOnLoginStatus() {
    if (isLoggedIn) {
        userEmailLabel->setText("Logged in as: " + currentUserEmail);
        loginButton->hide();
        registerButton->hide();
        aboutButton->hide();
        helpButton->hide();

        // Show logged-in UI
        startServiceButton->show();
        stopServiceButton->hide(); // Service not started yet
        logoutButton->show();
        logTextEdit->show();
    }
    else {
        userEmailLabel->setText("Welcome! Please login or register to access full features.");
        loginButton->show();
        registerButton->show();
        aboutButton->show();
        helpButton->show();

        // Hide logged-in UI
        startServiceButton->hide();
        stopServiceButton->hide();
        logoutButton->hide();
        logTextEdit->hide();
    }
}



void MainWindow::showLoginDialog() {
    LoginDialog loginDialog(this);
    if (loginDialog.exec() == QDialog::Accepted) {
        currentUserEmail = loginDialog.getLoggedInEmail();
        isLoggedIn = true;
        updateUIBasedOnLoginStatus();
        //clientService->setCurrentUserEmail(currentUserEmail);
    }
}

void MainWindow::showRegisterDialog() {
    RegisterDialog registerDialog(this);
    if (registerDialog.exec() == QDialog::Accepted) {
        updateUIBasedOnLoginStatus();
    }
}

void MainWindow::showAboutDialog() {
    // Create a QFile object to read the About content
    QFile aboutFile(":clientapp/content/about.txt");
    if (!aboutFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "Could not open About file.");
        return;
    }

    // Read the content of the About file
    QTextStream in(&aboutFile);
    QString aboutContent = in.readAll();
    aboutFile.close();

    QMessageBox::information(this, "About", aboutContent);
}

void MainWindow::showHelpDialog() {
    QFile helpFile(":clientapp/content/help.txt");
    if (!helpFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "Could not open Help file.");
        return;
    }

    QTextStream in(&helpFile);
    QString helpContent = in.readAll();
    helpFile.close();

    QMessageBox::information(this, "Help", helpContent);

}

void MainWindow::onLogoutClicked() {
    isLoggedIn = false;
    currentUserEmail.clear();
    updateUIBasedOnLoginStatus();
}

void MainWindow::updateLog(const QString& message) {
    logBuffer.append(message);
    //flushLogBuffer();
}

void MainWindow::flushLogBuffer() {
    if (!logBuffer.isEmpty()) {
        logTextEdit->setTextColor(QColor("black"));
        logTextEdit->append(logBuffer.join("\n"));
        logBuffer.clear();

        QScrollBar* sb = logTextEdit->verticalScrollBar();
        sb->setValue(sb->maximum());
    }
}


void MainWindow::onStartServiceClicked() {

    if (!clientService) {
        clientService = new ClientService(this);
        clientService = new ClientService();
        connect(clientService, &ClientService::logMessage, this, &MainWindow::updateLog);
    }

    clientService->startService();
    clientService->setCurrentUserEmail(currentUserEmail);
    startServiceButton->hide();
    stopServiceButton->show();
    startServiceButton->setEnabled(false);
    stopServiceButton->setEnabled(true);

    updateLog("Remote Control Service Started: " +
        QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
}

void MainWindow::onStopServiceClicked() {
    if (clientService) {
        clientService->stopService();
        startServiceButton->show();
        stopServiceButton->hide();
        updateLog("Remote Control Service Stopped: " +
            QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        startServiceButton->setEnabled(true);
        stopServiceButton->setEnabled(false);
    }
}

MainWindow::~MainWindow() {
    if (clientService) {
        delete clientService;
        clientService = nullptr;
    }
    logUpdateTimer->stop();
}