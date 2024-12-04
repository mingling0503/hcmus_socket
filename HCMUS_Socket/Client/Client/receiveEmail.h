#include "afxwin.h"  
#include "afxsock.h" 

#include "EAGetMailobj.tlh"
using namespace EAGetMailObjLib;

DWORD  _getCurrentPath(LPTSTR lpPath, DWORD nSize);
bool receiveEmail();