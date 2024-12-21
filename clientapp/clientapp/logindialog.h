// logindialog.h
#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>

class LoginDialog : public QDialog {
    Q_OBJECT

public:
    explicit LoginDialog(QWidget* parent = nullptr);
    QString getLoggedInEmail() const;
    //QString getCurrentUserEmail() const;

private slots:
    void validateLogin();
    void handleLogin();

private:
    bool authenticateUser(const QString& email, const QString& password);

    QLineEdit* emailLineEdit;
    QLineEdit* passwordLineEdit;
    QPushButton* loginButton;
    QString currentUserEmail;
};

#endif // LOGINDIALOG_H