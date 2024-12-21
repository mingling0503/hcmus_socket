#pragma once
// clientservice.h
#ifndef CLIENTSERVICE_H
#define CLIENTSERVICE_H

#include <QObject>
#include <QTimer>
#include <string>
#include <vector>
#include "emailHandling.h"
#include <QString>


class ClientService : public QObject {
    Q_OBJECT

public:
    explicit ClientService(QObject* parent = nullptr);
    void startService();
    void stopService();
    void setCurrentUserEmail(const QString& email);
    QString getCurrentUserEmail();


signals:
    void logMessage(const QString& message);

private:
    void processEmail();
    bool receiveEmail();
    vector<string> parseEmail(const string &emailFilePath, vector<string> &requests);
    bool sendEmailWith(std::string Recipient, std::string Subject, std::string& BodyText, const std::string& AttachmentPath="");

    QTimer* serviceTimer;
    QString currentUserEmail;
    string lastProcessedEmail;

    int executeRequest(const string& serverIP, const string& request,
        string& messageResult,
        string& resultFilePath,
        string& fileRequest
    );
};

#endif // CLIENTSERVICE_H