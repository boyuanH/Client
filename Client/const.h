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

#define CHECKHR  if(FAILED(hr)) return hr;