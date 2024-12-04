#include "afxwin.h"  // For MFC core and standard components
#include "afxsock.h" // For MFC socket classes (optional)

#include <Windows.h>

#include <tchar.h>
#include <Windows.h>
#include <string>

#include "easendmailobj.tlh"
using namespace EASendMailObjLib;

bool sendEmailWith(std::string Recipient, std::string Subject, std::string BodyText, std::string AttachmentPath);