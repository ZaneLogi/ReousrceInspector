// ResourceExtractor.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

typedef struct RESOURCE_PARAMS
{
    TCHAR inputFilePath[256];
    TCHAR outputFilePath[256];

    TCHAR targetType[256];
    TCHAR targetName[256];
    TCHAR targetLang[256];

    HANDLE fileHandle;
    DWORD sizeOfResource;
    bool found;

} RESOURCE_PARAMS;



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
    HRESULT hResult;

    RESOURCE_PARAMS* pParams = (RESOURCE_PARAMS*)lParam;

    // The type may be a string or an unsigned decimal integer
    if (!IS_INTRESOURCE(lpType))
    {
        hResult = StringCchPrintf(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), TEXT("%s"), lpType);
        if (FAILED(hResult))
        {
            // Add code to fail as securely as possible.
            return FALSE;
        }
    }
    else
    {
        hResult = StringCchPrintf(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), TEXT("0x%04x"), (USHORT)(LONG_PTR)lpType);
        if (FAILED(hResult))
        {
            // Add code to fail as securely as possible.
            return FALSE;
        }
    }

    if (_tcsicmp(szBuffer, pParams->targetType) != 0)
    {
        return TRUE;
    }

    // Find the names of all resources of type lpType.
    EnumResourceNames(hModule,
        lpType,
        EnumNamesFunc,
        lParam);

    return pParams->found ? FALSE : TRUE;
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
    HRESULT hResult;

    RESOURCE_PARAMS* pParams = (RESOURCE_PARAMS*)lParam;

    // The name may be a string or an unsigned decimal integer
    if (!IS_INTRESOURCE(lpName))
    {
        hResult = StringCchPrintf(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), TEXT("%s"), lpName);
        if (FAILED(hResult))
        {
            // Add code to fail as securely as possible.
            return FALSE;
        }
    }
    else
    {
        hResult = StringCchPrintf(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), TEXT("0x%04x"), (USHORT)(LONG_PTR)lpName);
        if (FAILED(hResult))
        {
            // Add code to fail as securely as possible.
            return FALSE;
        }
    }

    if (_tcsicmp(szBuffer, pParams->targetName) != 0)
    {
        return TRUE;
    }

    // Find the languages of all resources of type
    // lpType and name lpName.
    EnumResourceLanguages(hModule,
        lpType,
        lpName,
        EnumLangsFunc,
        lParam);

    return pParams->found ? FALSE : TRUE;
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
    HGLOBAL hResLoad;
    LPVOID lpResLock;

    TCHAR szBuffer[80];  // print buffer for info file
    DWORD cbWritten;     // number of bytes written to resource info file
    HRESULT hResult;

    RESOURCE_PARAMS* pParams = (RESOURCE_PARAMS*)lParam;

    hResInfo = FindResourceEx(hModule, lpType, lpName, wLang);
    // Write the resource language to the resource information file.
    hResult = StringCchPrintf(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), TEXT("0x%04x"), (USHORT)wLang);
    if (FAILED(hResult))
    {
        // Add code to fail as securely as possible.
        return FALSE;
    }

    if (_tcsicmp(szBuffer, pParams->targetLang) != 0)
    {
        return TRUE;
    }

    pParams->found = true;
    pParams->sizeOfResource = SizeofResource(hModule, hResInfo);

    hResLoad = LoadResource(hModule, hResInfo);
    lpResLock = LockResource(hResLoad);

    WriteFile(pParams->fileHandle, lpResLock, pParams->sizeOfResource, &cbWritten, NULL);

    return FALSE;
}







void ExtractExeResource(RESOURCE_PARAMS* pParams)
{
    HMODULE hExe;        // handle to .EXE file

    // Load the .EXE whose resources you want to list.
    hExe = LoadLibrary(pParams->inputFilePath);
    if (hExe == NULL)
    {
        // Add code to fail as securely as possible.
        return;
    }

    // Create a file to contain the resource info.
    pParams->fileHandle = CreateFile(pParams->outputFilePath,   // name of file
        GENERIC_READ | GENERIC_WRITE,      // access mode
        0,                                 // share mode
        (LPSECURITY_ATTRIBUTES)NULL,      // default security
        CREATE_ALWAYS,                     // create flags
        FILE_ATTRIBUTE_NORMAL,             // file attributes
        (HANDLE)NULL);                    // no template
    if (pParams->fileHandle == INVALID_HANDLE_VALUE)
    {
        _tprintf(TEXT("Could not open file."));
        return;
    }

    pParams->found = false;

    EnumResourceTypes(hExe,              // module handle
        (ENUMRESTYPEPROC)EnumTypesFunc,  // callback function
        (LONG_PTR)pParams);              // extra parameter

                                         // Unload the executable file whose resources were
                                         // enumerated and close the file created to contain
                                         // the resource information.
    FreeLibrary(hExe);
    CloseHandle(pParams->fileHandle);
}












int _tmain(int argc, TCHAR** argv)
{
    RESOURCE_PARAMS params;

    if (argc > 5)
    {
        StringCchCopy(params.inputFilePath, sizeof(params.inputFilePath) / sizeof(TCHAR), argv[1]);
        StringCchCopy(params.targetType, sizeof(params.targetType) / sizeof(TCHAR), argv[2]);
        StringCchCopy(params.targetName, sizeof(params.targetName) / sizeof(TCHAR), argv[3]);
        StringCchCopy(params.targetLang, sizeof(params.targetLang) / sizeof(TCHAR), argv[4]);
        StringCchCopy(params.outputFilePath, sizeof(params.outputFilePath) / sizeof(TCHAR), argv[5]);

        ExtractExeResource(&params);
    }

    return 0;
}

