#include "sendEmail.h"

bool sendEmailWith(std::string Recipient, std::string Subject, std::string BodyText, std::string AttachmentPath)
{
    ::CoInitialize(NULL);

    const int ConnectNormal = 0;
    const int ConnectSSLAuto = 1;
    const int ConnectSTARTTLS = 2;
    const int ConnectDirectSSL = 3;
    const int ConnectTryTLS = 4;

    IMailPtr oSmtp = NULL;
    oSmtp.CreateInstance(__uuidof(EASendMailObjLib::Mail));
    
    try {
        oSmtp->LicenseCode = _T("TryIt");
    }
    catch (const _com_error& e) {
        _tprintf(_T("COM Error: %s\n"), (const TCHAR*)e.Description());
        _tprintf(_T("Error Code: 0x%08lx\n"), e.Error());
    }

    // Set email address
    oSmtp->ServerAddr = _T("smtp.gmail.com");
    oSmtp->FromAddr = _T("almondsrabbit@gmail.com");
    oSmtp->AddRecipientEx(Recipient.c_str(), 0);

    // User and password for ESMTP authentication
    oSmtp->UserName = _T("almondsrabbit@gmail.com");
    oSmtp->Password = _T("slqi fvvq obfs giik");

    oSmtp->Subject = Subject.c_str();
    oSmtp->BodyText = BodyText.c_str();

    oSmtp->ConnectType = ConnectTryTLS;
    oSmtp->ServerPort = 587;

    // Add attachment
    oSmtp->AddAttachment(AttachmentPath.c_str());


    _tprintf(_T("Start to send email ...\r\n"));

    if (oSmtp->SendMail() == 0)
    {
        _tprintf(_T("Email was sent successfully!\r\n"));
    }
    else
    {
        _tprintf(_T("Failed to send email with the following error: %s\r\n"),
            (const TCHAR*)oSmtp->GetLastErrDescription());
        return false;
    }
    ::CoUninitialize();
    return true;
}
