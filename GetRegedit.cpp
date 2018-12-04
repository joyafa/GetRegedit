// GetManID.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>
#include <iostream>
#include <string>
#include <winnt.h>
using namespace std;

//HKEY_LOCAL_MACHINE\HARDWARE\DESCRIPTION\System\BIOS
void ReadRegValue(const HKEY hkey, const std::string& strKey)
{
	HKEY hkResult;
	cout << strKey << endl;
	LSTATUS lRet = RegOpenKeyEx(hkey, strKey.c_str(), 0, KEY_READ, &hkResult);

	if (ERROR_SUCCESS == lRet)
	{
		DWORD cSubKeys = 0;
		DWORD cbMaxSubKeyLen = 0;
		DWORD cbMaxClassLen = 0;
		DWORD cValues = 0;
		DWORD cbMaxValueNameLen = 0;
		DWORD cbMaxValueLen = 0;
		DWORD cbSecurityDescriptor = 0;
		FILETIME ftLastWriteTime;
		lRet = RegQueryInfoKey(
			    hkResult,
			    nullptr,
			    nullptr,
			    0,
				&cSubKeys,
				&cbMaxSubKeyLen,
				&cbMaxClassLen,
				&cValues,
				&cbMaxValueNameLen,
				&cbMaxValueLen,
				&cbSecurityDescriptor,
				&ftLastWriteTime
			);
		if (ERROR_SUCCESS == lRet)
		{
			if (cSubKeys > 0)
			{
				for (DWORD dwSubkey = 0; dwSubkey < cSubKeys; ++dwSubkey)
				{
					DWORD dwKeySize = cbMaxSubKeyLen + 1;
					TCHAR* lpKeyName = new TCHAR[dwKeySize];
					string subKey = strKey;
					memset(lpKeyName, 0, dwKeySize * sizeof(TCHAR));
					lRet = RegEnumKeyEx(hkResult, dwSubkey, lpKeyName, &dwKeySize, nullptr, nullptr, nullptr, nullptr);//读取键值
					if (ERROR_SUCCESS == lRet)
					{
						if (!subKey.empty())
						{
							subKey.append("\\");
						}
						subKey.append(lpKeyName);
						delete[] lpKeyName;
						ReadRegValue(hkey, subKey);
					}
					else
					{
						delete[] lpKeyName;
					}
				}
			}
			if (cValues > 0)
			{
				cout << "\t\t" << "Name" << "\t" << " Type" << "\t" << " Data"  << endl;
			}
			for (DWORD dwIndex = 0; dwIndex < cValues; ++dwIndex)
			{
				DWORD dwNameSize = cbMaxValueNameLen + 1;
				TCHAR* lpValueName = new TCHAR[dwNameSize];
				memset(lpValueName, 0, dwNameSize * sizeof(TCHAR));
				DWORD Type = 0;
				DWORD dwDataSize = cbMaxValueLen + 1;
				LPBYTE lpData = new BYTE[dwDataSize];
				memset(lpData, 0, dwDataSize * sizeof(BYTE));
				RegEnumValue(hkResult, dwIndex, lpValueName, &dwNameSize, nullptr, &Type, lpData, &dwDataSize);//读取键值
				if (dwNameSize > 0)
				{
					cout << "\t\t" << lpValueName << "\t" << Type << "\t" << lpData << endl;
				}
				delete[] lpValueName;
				delete[] lpData;
			}
		}
	}
}


#define MAKESTR(s) #s,s

void main(int argc, char* const* argv)
{
	if (argc != 2)
	{
		cout << "Usage: HKEY_LOCAL_MACHINE\\HARDWARE\\DESCRIPTION\\System";
		exit(1);
	}
	static struct
	{
		std::string strHkey;
		HKEY hkey;
	}Str2Key[] = {
		MAKESTR(HKEY_CLASSES_ROOT), 
		MAKESTR(HKEY_CURRENT_USER),
		MAKESTR(HKEY_LOCAL_MACHINE),
		MAKESTR(HKEY_USERS),
		MAKESTR(HKEY_PERFORMANCE_DATA),
		MAKESTR(HKEY_PERFORMANCE_TEXT),
		MAKESTR(HKEY_PERFORMANCE_NLSTEXT),
#if (WINVER >= 0x0400)
		MAKESTR(HKEY_CURRENT_CONFIG),
		MAKESTR(HKEY_CURRENT_CONFIG),
		MAKESTR(HKEY_CURRENT_USER_LOCAL_SETTINGS),
#endif
		"",0 };
	std::string strArgs = argv[1];
	std::string strHkey = strArgs;
	size_t pos = strArgs.find_first_of('\\');
	std::string strSubKey;
	if (string::npos != pos)
	{
		strHkey = strArgs.substr(0, pos);
		strSubKey = strArgs.substr(pos + 1);
	}
	HKEY hkey = 0;
	for (int i = 0; !Str2Key[i].strHkey.empty(); ++i)
	{
		if (Str2Key[i].strHkey == strHkey)
		{
			hkey = Str2Key[i].hkey;
			break;
		}
	}
	if (hkey)
	{
		ReadRegValue(hkey, strSubKey);
	}
}