#include "mainwindow.h"
#include <QMessageBox>
#include <QFile>
#include <QPalette>
#include <QScrollBar>
#include <QDateTime>
#include <QVBoxLayout>
#include <QTimer>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), logUpdateTimer(new QTimer(this)) {
    // Set a light background color
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(255, 248, 231));
    setPalette(palette);
    setAutoFillBackground(true);

    // Central widget and layout
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);
    layout->setContentsMargins(20, 40, 20, 40);
    layout->setSpacing(15);

    // Button styling
    QFont buttonFont;
    buttonFont.setPointSize(12);
    buttonFont.setBold(true);
    buttonFont.setFamily("San Francisco");

    registerMailButton = new QPushButton("Register Mail", this);
    startServiceButton = new QPushButton("Start Service", this);
    stopServiceButton = new QPushButton("Stop Service", this);

    QPushButton* buttons[] = { registerMailButton, startServiceButton, stopServiceButton };
    for (QPushButton* button : buttons) {
        button->setFont(buttonFont);
        button->setStyleSheet(
            "QPushButton {"
            "   background-color: #C2B280;"
            "   color: black;"
            "   border: 1px solid black;"
            "   padding: 10px;"
            "   border-radius: 5px;"
            "}"
            "QPushButton:hover {"
            "   background-color: #A67B5B;"
            "}"
        );
    }

    stopServiceButton->setEnabled(false);

    // Log text edit
    logTextEdit = new QTextEdit(this);
    logTextEdit->setReadOnly(true);
    logTextEdit->setFont(QFont("San Francisco", 11));
    logTextEdit->setStyleSheet(
        "QTextEdit {"
        "   background-color: #EED9C4;"
        "   border: 1px solid #E0E0E0;"
        "   border-radius: 5px;"
        "   padding: 10px;"
        "}"
    );

    // Add widgets to layout
    layout->addWidget(registerMailButton);
    layout->addWidget(startServiceButton);
    layout->addWidget(stopServiceButton);
    layout->addWidget(logTextEdit);

    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    // Initialize client service
    clientService = new ClientService(this);

    // Connect signals and slots
    connect(registerMailButton, &QPushButton::clicked, this, &MainWindow::onRegisterMailClicked);
    connect(startServiceButton, &QPushButton::clicked, this, &MainWindow::onStartServiceClicked);
    connect(stopServiceButton, &QPushButton::clicked, this, &MainWindow::onStopServiceClicked);
    connect(clientService, &ClientService::logMessage, this, &MainWindow::updateLog);

    // Timer for log updates
    connect(logUpdateTimer, &QTimer::timeout, this, &MainWindow::flushLogBuffer);
    logUpdateTimer->start(100); // Update log every 100ms

    setWindowTitle("Almonds Rabbit");
    resize(400, 600);
}

void MainWindow::updateLog(const QString& message) {
    logBuffer.append(message);
}

void MainWindow::flushLogBuffer() {
    if (!logBuffer.isEmpty()) {
        logTextEdit->setTextColor(QColor("black"));
        logTextEdit->append(logBuffer.join("\n"));
        logBuffer.clear();

        // Scroll to the bottom automatically
        QScrollBar* sb = logTextEdit->verticalScrollBar();
        sb->setValue(sb->maximum());
    }
}

void MainWindow::onRegisterMailClicked() {
    RegisterDialog registerDialog(this);
    if (registerDialog.exec() == QDialog::Accepted) {
        QFile registeredEmailsFile("registered_emails.txt");
        if (registeredEmailsFile.open(QIODevice::Append | QIODevice::Text)) {
            QTextStream out(&registeredEmailsFile);
            out << registerDialog.getRegisteredEmail() << "\n";
            registeredEmailsFile.close();
            updateLog("Email registered successfully: " + registerDialog.getRegisteredEmail());
        }
    }
}

void MainWindow::onStartServiceClicked() {
    startServiceButton->setEnabled(false);
    stopServiceButton->setEnabled(true);
    clientService->startService();
}

void MainWindow::onStopServiceClicked() {
    if (clientService) {
        clientService->stopService();
        updateLog("Remote Control Service Stopped: " +
            QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        startServiceButton->setEnabled(true);
        stopServiceButton->setEnabled(false);
    }
}

MainWindow::~MainWindow() {
    logUpdateTimer->stop();
}
