#pragma once

enum LoginType
{
	NORMAL,
	ADMIN,
	FORBID
};

struct OutputElement
{
	CString department;
	BOOL isDepartment;
	BOOL isOperation;
	BOOL isAlarm;
	BOOL isTrans;
};