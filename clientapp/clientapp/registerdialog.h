// registerdialog.h
#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QSet>

class RegisterDialog : public QDialog {
    Q_OBJECT

public:
    explicit RegisterDialog(QWidget* parent = nullptr);
    QString getRegisteredEmail() const;

private slots:
    void validateEmail();
    void handleRegistration();

private:
    QLineEdit* emailLineEdit;
    QPushButton* registerButton;
    QSet<QString> registeredEmails;  // New member to track registered emails

    bool isValidEmail(const QString& email) const;
    void loadRegisteredEmails();  // New method to load existing registrations
};

#endif // REGISTERDIALOG_H