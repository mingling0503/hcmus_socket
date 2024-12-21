//// logindialog.cpp

#include "logindialog.h"
#include "FontManager.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QCryptographicHash>
#include <QIcon>

LoginDialog::LoginDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle("Login");
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    setFixedSize(400, 200);

    setWindowIcon(QIcon("user.png"));

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(16);

    // Set the background color to beige
    setStyleSheet("background-color: #F5F5DC;");

    // Email input
    emailLineEdit = new QLineEdit();
    emailLineEdit->setPlaceholderText("Email");
    emailLineEdit->setStyleSheet("font-size: 15px; padding: 8px; background-color: #F5F5DC; color: black; " );
    emailLineEdit->setFont(FontManager::getInstance().getLogFont());
    // Password input
    passwordLineEdit = new QLineEdit();
    passwordLineEdit->setPlaceholderText("Password");
    passwordLineEdit->setEchoMode(QLineEdit::Password);
    passwordLineEdit->setStyleSheet("font-size: 15px; padding: 8px; background-color: #F5F5DC; color: black;");
    passwordLineEdit->setFont(FontManager::getInstance().getLogFont());
    // Login button
    loginButton = new QPushButton("Login");
    loginButton->setStyleSheet("font-size: 15px; padding: 8px; background-color: #C2B280; color: black;");
    loginButton->setEnabled(false);
    loginButton->setFont(FontManager::getInstance().getButtonFont());

    layout->addWidget(emailLineEdit);
    layout->addWidget(passwordLineEdit);
    layout->addWidget(loginButton);

    connect(emailLineEdit, &QLineEdit::textChanged, this, &LoginDialog::validateLogin);
    connect(passwordLineEdit, &QLineEdit::textChanged, this, &LoginDialog::validateLogin);
    connect(loginButton, &QPushButton::clicked, this, &LoginDialog::handleLogin);
}

void LoginDialog::validateLogin() {
    QString email = emailLineEdit->text();
    QString password = passwordLineEdit->text();
    loginButton->setEnabled(!email.isEmpty() && !password.isEmpty());
}

void LoginDialog::handleLogin() {
    QString email = emailLineEdit->text();
    QString password = passwordLineEdit->text();

    
    if (authenticateUser(email, password)) {
        currentUserEmail = email;
        QMessageBox msgBox(this);
        msgBox.setFont(FontManager::getInstance().getLogFont());
        msgBox.setWindowTitle("Login Successful");
        msgBox.setText("Welcome, " + email);
        msgBox.setIcon(QMessageBox::Information);

       
        msgBox.setStyleSheet("QMessageBox { background-color: #f5f5dc; color: black; } "
            "QMessageBox QLabel { color: black; } "
            "QPushButton { background-color: #d2b48c; color: black; border-radius: 5px; padding: 5px; } "
            "QPushButton:hover { background-color: #c4a484; }");

        msgBox.exec();
        accept();
    }
    else {
        
        QMessageBox msgBox(this);
        msgBox.setFont(FontManager::getInstance().getLogFont());
        msgBox.setWindowTitle("Login Failed");
        msgBox.setText("Invalid email or password");
        msgBox.setIcon(QMessageBox::Warning);

       
        msgBox.setStyleSheet("QMessageBox { background-color: #f5f5dc; color: black; } "
            "QMessageBox QLabel { color: black; font: } "
            "QPushButton { background-color: #d2b48c; color: black; border-radius: 5px; padding: 5px; } "
            "QPushButton:hover { background-color: #c4a484; }");

        msgBox.exec();
        passwordLineEdit->clear();
    }
}


bool LoginDialog::authenticateUser(const QString& email, const QString& password) {
    QFile file("registered_users.json");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject usersObj = doc.object();

    // Check if email exists
    if (!usersObj.contains(email)) {
        return false;
    }

    // Hash the input password
    QByteArray hashedPassword = QCryptographicHash::hash(
        password.toUtf8(),
        QCryptographicHash::Sha256
    );
    QString hashedPasswordHex = QString::fromUtf8(hashedPassword.toHex());

    // Compare hashed passwords
    return usersObj[email].toString() == hashedPasswordHex;
}

QString LoginDialog::getLoggedInEmail() const {
    return emailLineEdit->text();
}
