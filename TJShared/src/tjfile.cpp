#include "../include/tjshared.h"
#include "shlwapi.h"
using namespace tj::shared;

std::wstring File::GetDirectory(const std::wstring& pathToFile) {
	wchar_t* buf = _wcsdup(pathToFile.c_str());
	PathRemoveFileSpec(buf);
	std::wstring dir = buf;
	delete[] buf;
	return dir;
}

bool File::Exists(const std::wstring& st) {
	return GetFileAttributes(st.c_str())!=INVALID_FILE_ATTRIBUTES;
}

Bytes File::GetDirectorySize(const std::wstring& dirPath) {
	WIN32_FIND_DATA fd;
	std::wstring searchPath = dirPath+L"*";
	HANDLE search = FindFirstFile(searchPath.c_str(), &fd);

	if(search==INVALID_HANDLE_VALUE) {
		Log::Write(L"TJShared/File", L"Could not get directory size for '"+dirPath+L"'");
		return 0;
	}

	LARGE_INTEGER totalSize;
	totalSize.QuadPart = 0;

	do {
		LARGE_INTEGER size;
		size.LowPart = fd.nFileSizeLow;
		size.HighPart = fd.nFileSizeHigh;
		totalSize.QuadPart += size.QuadPart;
	} 
	while(FindNextFile(search, &fd));

	FindClose(search);

	// TODO: what to do when size exceeds 4GB (largest number in unsigned int)?
	return (Bytes)totalSize.QuadPart;
}