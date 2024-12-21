// registerdialog.cpp
#include "registerdialog.h"
#include "FontManager.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QRegularExpression>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QSet>
#include <QJsonObject>
#include <QJsonDocument>
#include <QCryptographicHash>

RegisterDialog::RegisterDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle("Register Account");
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    setFixedSize(400, 300);

    setWindowIcon(QIcon("user.png"));

    // Set the background color to beige
    setStyleSheet("background-color: #F5F5DC;");

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(16);

    // Email input
    emailLineEdit = new QLineEdit(this);
    emailLineEdit->setPlaceholderText("Email");
    emailLineEdit->setStyleSheet("font-size: 15px; padding: 8px; background-color: #F5F5DC; color: black;");
    emailLineEdit->setFont(FontManager::getInstance().getLogFont());

    // Password input
    passwordLineEdit = new QLineEdit(this);
    passwordLineEdit->setPlaceholderText("Password");
    passwordLineEdit->setEchoMode(QLineEdit::Password);
    passwordLineEdit->setStyleSheet("font-size: 15px; padding: 8px; background-color: #F5F5DC; color: black;");
    passwordLineEdit->setFont(FontManager::getInstance().getLogFont());

    // Confirm Password input
    confirmPasswordLineEdit = new QLineEdit(this);
    confirmPasswordLineEdit->setPlaceholderText("Confirm Password");
    confirmPasswordLineEdit->setEchoMode(QLineEdit::Password);
    confirmPasswordLineEdit->setStyleSheet("font-size: 15px; padding: 8px; background-color: #F5F5DC; color: black;");
    confirmPasswordLineEdit->setFont(FontManager::getInstance().getLogFont());

    // Register button
    registerButton = new QPushButton("Register", this);
    registerButton->setStyleSheet("font-size: 15px; padding: 8px; background-color: #C2B280; color: black;");
    registerButton->setEnabled(true);
    registerButton->setFont(FontManager::getInstance().getButtonFont());

    // Add widgets to layout
    layout->addWidget(emailLineEdit);
    layout->addWidget(passwordLineEdit);
    layout->addWidget(confirmPasswordLineEdit);
    layout->addWidget(registerButton);

    // Connections remain the same
    connect(emailLineEdit, &QLineEdit::textChanged, this, &RegisterDialog::validateRegistration);
    connect(passwordLineEdit, &QLineEdit::textChanged, this, &RegisterDialog::validateRegistration);
    connect(confirmPasswordLineEdit, &QLineEdit::textChanged, this, &RegisterDialog::validateRegistration);
    connect(registerButton, &QPushButton::clicked, this, &RegisterDialog::handleRegistration);
}

void RegisterDialog::loadRegisteredUsers() {
    QFile file("registered_users.json");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QByteArray data = file.readAll();
        file.close();

        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonObject usersObj = doc.object();

        for (auto it = usersObj.begin(); it != usersObj.end(); ++it) {
            registeredUsers.insert(it.key());
        }
    }
}

bool RegisterDialog::isValidEmail(const QString& email) const {
    QRegularExpression emailRegex(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
    return emailRegex.match(email).hasMatch();
}

bool RegisterDialog::isValidPassword(const QString& password) const {
    // Password must be at least 8 characters, contain a number and a special character
    return password.length() >= 8 &&
        password.contains(QRegularExpression("[0-9]")) &&
        password.contains(QRegularExpression("[!@#$%^&*(),.?\":{}|<>]"));
}

void RegisterDialog::validateRegistration() {
    loadRegisteredUsers();
    registerButton->setEnabled(true);
}

void RegisterDialog::handleRegistration() {

    QString email = emailLineEdit->text();
    QString password = passwordLineEdit->text();
    QString confirmPassword = confirmPasswordLineEdit->text();

    QString errorMessage;

    if (email.isEmpty()) {
        errorMessage += "- Email cannot be empty\n";
    }
    else if (!isValidEmail(email)) {
        errorMessage += "- Invalid email format\n";
    }

    
    if (password.isEmpty()) {
        errorMessage += "- Password cannot be empty\n";
    }
    else {
        if (password.length() < 8) {
            errorMessage += "- Password must be at least 8 characters long\n";
        }

        if (!password.contains(QRegularExpression("[0-9]"))) {
            errorMessage += "- Password must contain at least one number\n";
        }

        if (!password.contains(QRegularExpression("[!@#$%^&*(),.?\":{}|<>]"))) {
            errorMessage += "- Password must contain at least one special character\n";
        }
    }

    if (confirmPassword.isEmpty()) {
        errorMessage += "- Confirm Password cannot be empty\n";
    }
    else if (password != confirmPassword) {
        errorMessage += "- Passwords do not match\n";
    }

    if (registeredUsers.contains(email)) {
        errorMessage += "- This email is already registered\n";
    }

    if (!errorMessage.isEmpty()) {
        QMessageBox msgBox(this);
        msgBox.setFont(FontManager::getInstance().getLogFont());
        msgBox.setWindowTitle("Registration Error");
        msgBox.setText("Please correct the following errors:\n" + errorMessage);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStyleSheet("QMessageBox { background-color: #f5f5dc; color: black; } "
            "QMessageBox QLabel { color: black; } "
            "QPushButton { background-color: #d2b48c; color: black; border-radius: 5px; padding: 5px; } "
            "QPushButton:hover { background-color: #c4a484; }");
        msgBox.exec();
        return;
    }

    // Nếu không có lỗi, tiến hành đăng kí
    // Hash the password
    QByteArray hashedPassword = QCryptographicHash::hash(
        password.toUtf8(),
        QCryptographicHash::Sha256
    );

    // Read existing JSON
    QJsonObject usersObj;
    QFile readFile("registered_users.json");
    if (readFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QByteArray data = readFile.readAll();
        readFile.close();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        usersObj = doc.object();
    }

    // Add new user
    QString hashedPasswordHex = QString::fromUtf8(hashedPassword.toHex());
    usersObj[email] = hashedPasswordHex;

    // Write updated JSON
    QFile writeFile("registered_users.json");
    if (writeFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QJsonDocument doc(usersObj);
        writeFile.write(doc.toJson());
        writeFile.close();

        registeredUsers.insert(email);

        QMessageBox msgBox(this);
        msgBox.setFont(FontManager::getInstance().getLogFont());
        msgBox.setWindowTitle("Registration Successful");
        msgBox.setText("Account has been successfully registered.");
        msgBox.setIcon(QMessageBox::Information);


        msgBox.setStyleSheet("QMessageBox { background-color: #f5f5dc; color: black; } "
            "QMessageBox QLabel { color: black; } "
            "QPushButton { background-color: #d2b48c; color: black; border-radius: 5px; padding: 5px; } "
            "QPushButton:hover { background-color: #c4a484; }");
        msgBox.exec();
        accept();
    }
    else {
        QMessageBox::critical(this, "Error",
            "Could not open file for registration. Please try again.");
    }
}

QString RegisterDialog::getRegisteredEmail() const {
    return emailLineEdit->text();
}