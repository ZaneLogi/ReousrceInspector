// ResourceUpdator.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

typedef struct RESOURCE_PARAMS
{
    TCHAR inputFilePath[256];
    TCHAR binaryFilePath[256];

    TCHAR targetType[256];
    TCHAR targetName[256];
    TCHAR targetLang[256];

    LPVOID resourceBinary;
    DWORD sizeOfResourceBinary;
    bool found;

} RESOURCE_PARAMS;


void UpdateExeResource(RESOURCE_PARAMS* pParams)
{
    HANDLE hExe;        // handle to .EXE file
    HANDLE hBinary;
    DWORD numberOfBytesRead;

    hExe = BeginUpdateResource(pParams->inputFilePath, FALSE);
    if (hExe == NULL)
    {
        // Add code to fail as securely as possible.
        return;
    }

    // Create a file to contain the resource info.
    hBinary = CreateFile(pParams->binaryFilePath,   // name of file
        GENERIC_READ,      // access mode
        FILE_SHARE_READ,                                 // share mode
        (LPSECURITY_ATTRIBUTES)NULL,      // default security
        OPEN_EXISTING,                     // create flags
        FILE_ATTRIBUTE_NORMAL,             // file attributes
        (HANDLE)NULL);                    // no template
    if (hBinary == INVALID_HANDLE_VALUE)
    {
        _tprintf(TEXT("Could not open file."));
        return;
    }

    pParams->sizeOfResourceBinary = GetFileSize(hBinary, NULL);
    pParams->resourceBinary = malloc(pParams->sizeOfResourceBinary);
    if (pParams->resourceBinary == NULL)
    {
        _tprintf(TEXT("Failed to allocate memory. size = %d"), pParams->sizeOfResourceBinary);
        CloseHandle(hBinary);
        return;
    }

    ReadFile(hBinary, pParams->resourceBinary, pParams->sizeOfResourceBinary, &numberOfBytesRead, NULL);

    CloseHandle(hBinary);

    pParams->found = false;

    EndUpdateResource(hExe, FALSE);
    free(pParams->resourceBinary);
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
        StringCchCopy(params.binaryFilePath, sizeof(params.binaryFilePath) / sizeof(TCHAR), argv[5]);

        UpdateExeResource(&params);
    }

    return 0;
}

