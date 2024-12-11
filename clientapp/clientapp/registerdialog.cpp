// registerdialog.cpp
#include "registerdialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QRegularExpression>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QSet>

RegisterDialog::RegisterDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle("Register Email");

    // Load previously registered emails
    loadRegisteredEmails();

    QVBoxLayout* layout = new QVBoxLayout(this);

    QLabel* emailLabel = new QLabel("Enter Email to Register:", this);
    emailLineEdit = new QLineEdit(this);
    registerButton = new QPushButton("Register", this);
    registerButton->setEnabled(false);

    layout->addWidget(emailLabel);
    layout->addWidget(emailLineEdit);
    layout->addWidget(registerButton);

    connect(emailLineEdit, &QLineEdit::textChanged, this, &RegisterDialog::validateEmail);
    connect(registerButton, &QPushButton::clicked, this, &RegisterDialog::handleRegistration);
}

void RegisterDialog::loadRegisteredEmails() {
    // Load registered emails from a file
    QFile file("registered_emails.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString email = in.readLine().trimmed();
            registeredEmails.insert(email);
        }
        file.close();
    }
}

bool RegisterDialog::isValidEmail(const QString& email) const {
    QRegularExpression emailRegex(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
    return emailRegex.match(email).hasMatch();
}

void RegisterDialog::validateEmail() {
    QString email = emailLineEdit->text();
    registerButton->setEnabled(isValidEmail(email));
}

void RegisterDialog::handleRegistration() {
    QString email = emailLineEdit->text();

    // Check if email is already registered
    if (registeredEmails.contains(email)) {
        // Show warning message if email already exists
        QMessageBox::warning(this, "Registration Error",
            "This email is already registered. Please use a different email.");

        // Clear the line edit and disable register button
        emailLineEdit->clear();
        registerButton->setEnabled(false);
        return;
    }

    // If email is not registered, proceed with registration
    QFile file("registered_emails.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << email << "\n";
        file.close();

        // Add to the in-memory set of registered emails
        registeredEmails.insert(email);

        // Show success message
        QMessageBox::information(this, "Registration Successful",
            "Email has been successfully registered.");

        // Accept the dialog
        accept();
    }
    else {
        // Handle file opening error
        QMessageBox::critical(this, "Error",
            "Could not open file for registration. Please try again.");
    }
}

QString RegisterDialog::getRegisteredEmail() const {
    return emailLineEdit->text();
}