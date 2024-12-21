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
    void validateRegistration();
    void handleRegistration();

private:
    void loadRegisteredUsers();
    bool isValidEmail(const QString& email) const;
    bool isValidPassword(const QString& password) const;

    QLineEdit* emailLineEdit;
    QLineEdit* passwordLineEdit;
    QLineEdit* confirmPasswordLineEdit;
    QPushButton* registerButton;
    QSet<QString> registeredUsers;
};

#endif // REGISTERDIALOG_H