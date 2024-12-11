#pragma once
// clientservice.h
#ifndef CLIENTSERVICE_H
#define CLIENTSERVICE_H

#include <QObject>
#include <QTimer>
#include <string>
#include <vector>
#include "emailHandling.h"


class ClientService : public QObject {
    Q_OBJECT

public:
    explicit ClientService(QObject* parent = nullptr);
    void startService();
    void stopService();

signals:
    void logMessage(const QString& message);

private:
    void processEmail();
    bool receiveEmail();
    vector<string> parseEmail(const string &emailFilePath, vector<string> &requests);
    int executeRequest(const string& serverIP, const string& request, 
        string& messageResult, 
        const string& resultFilePath
    );
    bool sendEmailWith(std::string Recipient, std::string Subject, std::string& BodyText, const std::string& AttachmentPath="");

    QTimer* serviceTimer;
    string lastProcessedEmail;
};

#endif // CLIENTSERVICE_H