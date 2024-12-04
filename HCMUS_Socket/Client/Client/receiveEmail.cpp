#include "receiveEmail.h"
#include <iostream>

DWORD  _getCurrentPath(LPTSTR lpPath, DWORD nSize)
{
    DWORD dwSize = ::GetModuleFileName(NULL, lpPath, nSize);
    if (dwSize == 0 || dwSize == nSize)
    {
        return 0;
    }

    // Change file name to current full path
    LPCTSTR psz = _tcsrchr(lpPath, _T('\\'));
    if (psz != NULL)
    {
        lpPath[psz - lpPath] = _T('\0');
        return _tcslen(lpPath);
    }

    return 0;
}

bool receiveEmail()
{
    const int MailServerPop3 = 0;
    const int MailServerImap4 = 1;
    const int MailServerEWS = 2;
    const int MailServerDAV = 3;
    const int MailServerMsGraph = 4;

    bool received = false;

    // Initialize COM environment
    ::CoInitialize(NULL);

    // Create a folder named "inbox" under current exe file directory
    // to save the emails retrieved.

    TCHAR szPath[MAX_PATH + 1];
    _getCurrentPath(szPath, MAX_PATH);

    TCHAR szMailBox[MAX_PATH + 1];
    wsprintf(szMailBox, _T("%s\\inbox"), szPath);

    TCHAR szNewFile[MAX_PATH + 1];
    wsprintf(szNewFile, _T("%s\\email.txt"), szMailBox);

    if (GetFileAttributes(szNewFile) != INVALID_FILE_ATTRIBUTES) {
        if (!DeleteFile(szNewFile)) {
            _tprintf(_T("Failed to delete existing email.txt. Error: %d\n"), GetLastError());
            return false;
        }
    }

    // Create a folder to store emails
    ::CreateDirectory(szMailBox, NULL);

    try
    {
        IMailServerPtr oServer = NULL;
        oServer.CreateInstance(__uuidof(EAGetMailObjLib::MailServer));

        // Gmail IMAP4 server address
        oServer->Server = _T("imap.gmail.com");
        oServer->User = _T("almondsrabbit@gmail.com");

        // Create app password in Google account
        oServer->Password = _T("slqi fvvq obfs giik");
        oServer->Protocol = MailServerImap4;

        // Enable SSL connection
        oServer->SSLConnection = VARIANT_TRUE;

        // Set 993 IMAP4 SSL port
        oServer->Port = 993;

        IMailClientPtr oClient = NULL;
        oClient.CreateInstance(__uuidof(EAGetMailObjLib::MailClient));
        oClient->LicenseCode = _T("TryIt");

        if (oClient->Connect(oServer)) {
            _tprintf(_T("Connected\r\n"));
        }

        IMailInfoCollectionPtr infos = oClient->GetMailInfoList();
        if (infos->Count > 0)
            std::cout << "Email received!\n";

        for (long i = 0; i < infos->Count; i++)
        {
            IMailInfoPtr pInfo = infos->GetItem(i);

            TCHAR szFile[MAX_PATH + 1];
            // Generate a random file name by current local datetime,
            // You can use your method to generate the filename if you do not like it
            SYSTEMTIME curtm;
            ::GetLocalTime(&curtm);
            ::wsprintf(szFile, _T("%s\\%04d%02d%02d%02d%02d%02d%03d%d.txt"),
                szMailBox,
                curtm.wYear,
                curtm.wMonth,
                curtm.wDay,
                curtm.wHour,
                curtm.wMinute,
                curtm.wSecond,
                curtm.wMilliseconds,
                i);

            // Receive email from POP3 server
            IMailPtr oMail = oClient->GetMail(pInfo);
            received = true;

            // Save email to local disk
            oMail->SaveAs(szFile, VARIANT_TRUE);
            // Rename the saved file to "email.txt"

            TCHAR szNewFile[MAX_PATH + 1];
            wsprintf(szNewFile, _T("%s\\email.txt"), szMailBox);

            if (!MoveFileEx(szFile, szNewFile, MOVEFILE_REPLACE_EXISTING)) {
                _tprintf(_T("Failed to rename file to email.txt. Error: %d\n"), GetLastError());
                return false;
            }

            // Mark email as deleted from POP3 server.
            oClient->Delete(pInfo);
        }

        // Delete method just mark the email as deleted,
        // Quit method expunge the emails from server exactly.
        oClient->Quit();
    }
    catch (_com_error& ep)
    {
        _tprintf(_T("Error: %s"), (const TCHAR*)ep.Description());
    }
    return received;
}
