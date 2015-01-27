#pragma once
#include <vector>
enum LoginType
{
	NORMAL,
	ADMIN,
	FORBID
};

// struct OutputElement
// {
// 	CString department;
// 	BOOL isDepartment;
// 	BOOL isOperation;
// 	BOOL isAlarm;
// 	BOOL isTrans;
// };

struct DatabaseInfo
{
	CString server;
	CString database;
	CString username;
	CString password;
};

struct FileUint
{
	CString fileName;
	BOOL isCheck;
};

struct OutUnit
{
	CString departName;
	BOOL isDepartment;	
	std::vector<FileUint> fileEle;
};

struct OutputInfo
{
	HWND hWnd;
	CString m_server;CString m_database;CString m_username;CString m_password;
	CString m_FolderPath;COleDateTime m_startDateTime;COleDateTime m_endDateTime;
	std::vector<OutUnit> m_outputEle;
};

#define CHECKHR  if(FAILED(hr)) return hr;
#define CHK_HR(stmt)        do { hr=(stmt); if (FAILED(hr)) goto CleanUp; } while(0)

// Macro to verify memory allcation.
#define CHK_ALLOC(p)        do { if (!(p)) { hr = E_OUTOFMEMORY; goto CleanUp; } } while(0)

// Macro that releases a COM object if not NULL.
#define SAFE_RELEASE(p)     do { if ((p)) { (p)->Release(); (p) = NULL; } } while(0)