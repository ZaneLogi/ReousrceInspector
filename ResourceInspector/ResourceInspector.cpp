// ResourceInspector.cpp : Defines the entry point for the console application.
//
// https://msdn.microsoft.com/en-us/library/windows/desktop/ms648008(v=vs.85).aspx#_win32_Updating_Resources

#include "stdafx.h"


HANDLE g_hFile;   // global handle to resource info file

// Declare callback functions.
BOOL CALLBACK EnumTypesFunc(
    HMODULE hModule,
    LPTSTR lpType,
    LONG_PTR lParam);

BOOL CALLBACK EnumNamesFunc(
    HMODULE hModule,
    LPCTSTR lpType,
    LPTSTR lpName,
    LONG_PTR lParam);

BOOL CALLBACK EnumLangsFunc(
    HMODULE hModule,
    LPCTSTR lpType,
    LPCTSTR lpName,
    WORD wLang,
    LONG_PTR lParam);



//    FUNCTION: EnumTypesFunc(HANDLE, LPSTR, LONG)
//
//    PURPOSE:  Resource type callback
BOOL CALLBACK EnumTypesFunc(
    HMODULE hModule,  // module handle
    LPTSTR lpType,    // address of resource type
    LONG_PTR lParam)      // extra parameter, could be
                      // used for error checking
{
    TCHAR szBuffer[80];  // print buffer for info file
    DWORD cbWritten;     // number of bytes written to resource info file
    size_t cbString;
    HRESULT hResult;

    // Write the resource type to a resource information file.
    // The type may be a string or an unsigned decimal
    // integer, so test before printing.
    if (!IS_INTRESOURCE(lpType))
    {
        hResult = StringCchPrintf(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), TEXT("Type: %s\r\n"), lpType);
        if (FAILED(hResult))
        {
            // Add code to fail as securely as possible.
            return FALSE;
        }
    }
    else
    {
        hResult = StringCchPrintf(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), TEXT("Type: 0x%04x\r\n"), (USHORT)(LONG_PTR)lpType);
        if (FAILED(hResult))
        {
            // Add code to fail as securely as possible.
            return FALSE;
        }
    }

    hResult = StringCbLength(szBuffer, sizeof(szBuffer), &cbString);
    if (FAILED(hResult))
    {
        // Add code to fail as securely as possible.
        return FALSE;
    }

    WriteFile(g_hFile, szBuffer, (DWORD)cbString, &cbWritten, NULL);
    // Find the names of all resources of type lpType.
    EnumResourceNames(hModule,
        lpType,
        EnumNamesFunc,
        0);

    return TRUE;
}

//    FUNCTION: EnumNamesFunc(HANDLE, LPSTR, LPSTR, LONG)
//
//    PURPOSE:  Resource name callback
BOOL CALLBACK EnumNamesFunc(
    HMODULE hModule,  // module handle
    LPCTSTR lpType,   // address of resource type
    LPTSTR lpName,    // address of resource name
    LONG_PTR lParam)      // extra parameter, could be
                      // used for error checking
{
    TCHAR szBuffer[80];  // print buffer for info file
    DWORD cbWritten;     // number of bytes written to resource info file
    size_t cbString;
    HRESULT hResult;

    // Write the resource name to a resource information file.
    // The name may be a string or an unsigned decimal
    // integer, so test before printing.
    if (!IS_INTRESOURCE(lpName))
    {
        hResult = StringCchPrintf(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), TEXT("\tName: %s\r\n"), lpName);
        if (FAILED(hResult))
        {
            // Add code to fail as securely as possible.
            return FALSE;
        }
    }
    else
    {
        hResult = StringCchPrintf(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), TEXT("\tName: 0x%04x\r\n"), (USHORT)(LONG_PTR)lpName);
        if (FAILED(hResult))
        {
            // Add code to fail as securely as possible.
            return FALSE;
        }
    }

    hResult = StringCbLength(szBuffer, sizeof(szBuffer), &cbString);
    if (FAILED(hResult))
    {
        // Add code to fail as securely as possible.
        return FALSE;
    }

    WriteFile(g_hFile, szBuffer, (DWORD)cbString, &cbWritten, NULL);
    // Find the languages of all resources of type
    // lpType and name lpName.
    EnumResourceLanguages(hModule,
        lpType,
        lpName,
        EnumLangsFunc,
        0);

    return TRUE;
}

//    FUNCTION: EnumLangsFunc(HANDLE, LPSTR, LPSTR, WORD, LONG)
//
//    PURPOSE:  Resource language callback
BOOL CALLBACK EnumLangsFunc(
    HMODULE hModule, // module handle
    LPCTSTR lpType,  // address of resource type
    LPCTSTR lpName,  // address of resource name
    WORD wLang,      // resource language
    LONG_PTR lParam)     // extra parameter, could be
                     // used for error checking
{
    HRSRC hResInfo;
    TCHAR szBuffer[80];  // print buffer for info file
    DWORD cbWritten;     // number of bytes written to resource info file
    size_t cbString;
    HRESULT hResult;

    hResInfo = FindResourceEx(hModule, lpType, lpName, wLang);
    // Write the resource language to the resource information file.
    hResult = StringCchPrintf(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), TEXT("\t\tLanguage: 0x%04x\r\n"), (USHORT)wLang);
    if (FAILED(hResult))
    {
        // Add code to fail as securely as possible.
        return FALSE;
    }

    hResult = StringCbLength(szBuffer, sizeof(szBuffer), &cbString);
    if (FAILED(hResult))
    {
        // Add code to fail as securely as possible.
        return FALSE;
    }

    WriteFile(g_hFile, szBuffer, (DWORD)cbString, &cbWritten, NULL);
    // Write the resource handle and size to buffer.
    hResult = StringCchPrintf(szBuffer,
        sizeof(szBuffer) / sizeof(TCHAR),
        TEXT("\t\thResInfo == %lx,  Size == %lu\r\n\r\n"),
        hResInfo,
        SizeofResource(hModule, hResInfo));
    if (FAILED(hResult))
    {
        // Add code to fail as securely as possible.
        return FALSE;
    }

    hResult = StringCbLength(szBuffer, sizeof(szBuffer), &cbString);
    if (FAILED(hResult))
    {
        // Add code to fail as securely as possible.
        return FALSE;
    }

    WriteFile(g_hFile, szBuffer, (DWORD)cbString, &cbWritten, NULL);
    return TRUE;
}

void InspectExeResources(LPCTSTR infilePath, LPCTSTR outfilePath)
{
    HMODULE hExe;        // handle to .EXE file
    TCHAR szBuffer[256];  // print buffer for info file
    DWORD cbWritten;     // number of bytes written to resource info file
    size_t cbString;     // length of string in szBuffer
    HRESULT hResult;

    // Load the .EXE whose resources you want to list.
    hExe = LoadLibrary(infilePath);
    if (hExe == NULL)
    {
        // Add code to fail as securely as possible.
        return;
    }

    // Create a file to contain the resource info.
    g_hFile = CreateFile(outfilePath,   // name of file
        GENERIC_READ | GENERIC_WRITE,      // access mode
        0,                                 // share mode
        (LPSECURITY_ATTRIBUTES)NULL,      // default security
        CREATE_ALWAYS,                     // create flags
        FILE_ATTRIBUTE_NORMAL,             // file attributes
        (HANDLE)NULL);                    // no template
    if (g_hFile == INVALID_HANDLE_VALUE)
    {
        _tprintf(TEXT("Could not open file."));
        return;
    }


    // write 0xff 0xfe
    USHORT tag = 0xfeff;
    WriteFile(g_hFile,       // file to hold resource info
        &tag,            // what to write to the file
        (DWORD)sizeof(tag),    // number of bytes in szBuffer
        &cbWritten,          // number of bytes written
        NULL);               // no overlapped I/O


    // Find all of the loaded file's resources.
    hResult = StringCchPrintf(szBuffer, sizeof(szBuffer) / sizeof(TCHAR),
        TEXT("%s\r\nThe file contains the following resources:\r\n\r\n"),
        infilePath);
    if (FAILED(hResult))
    {
        // Add code to fail as securely as possible.
        return;
    }

    hResult = StringCbLength(szBuffer, sizeof(szBuffer), &cbString);
    if (FAILED(hResult))
    {
        // Add code to fail as securely as possible.
        return;
    }

    WriteFile(g_hFile,       // file to hold resource info
        szBuffer,            // what to write to the file
        (DWORD)cbString,    // number of bytes in szBuffer
        &cbWritten,          // number of bytes written
        NULL);               // no overlapped I/O

    EnumResourceTypes(hExe,              // module handle
        EnumTypesFunc,                   // callback function
        0);                              // extra parameter

                                         // Unload the executable file whose resources were
                                         // enumerated and close the file created to contain
                                         // the resource information.
    FreeLibrary(hExe);
    CloseHandle(g_hFile);
}






int _tmain(int argc, TCHAR** argv)
{
    if (argc > 2)
    {
        InspectExeResources(argv[1], argv[2]);
    }
    else if (argc > 1)
    {
        InspectExeResources(argv[1], TEXT("resinfo.txt"));
    }
    return 0;
}

