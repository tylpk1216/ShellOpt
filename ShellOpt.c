#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <Library/UefiLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/PrintLib.h>

bool isNumber(char *str)
{
    char *p = str;

    while (*p) {
        if (*p < '0' || *p > '9') {
            return false;
        }
        p++;
    }

    return true;
}

bool parseArgv(int argc, char *argv[], int bufferLen, char *buffer)
{
    int i, j;

    int totalLen = bufferLen;
    int len = 0;

    char *items[] = {
        "-nostartup",
        "-noconsoleout",
        "-noconsolein",
        "-delay",
        "-nointerrupt",
        "-nomap",
        "-noversion",
        "-startup",
        "-nonesting",
        "-exit"
    };

    int itemsCount = sizeof(items) / sizeof(char*);
    bool found = false;

    memset(buffer, 0, bufferLen);

    for (i = 1; i < argc; i++) {
        len = (int)strlen(argv[i]);

        // append space behind previous argv
        if (i > 1) {
            strncat(buffer, " ", totalLen);
            totalLen -= 1;
        }

        found = false;
        for (j = 0; j < itemsCount; j++) {
            if (strcmp(argv[i], items[j]) == 0) {
                printf("Add (%s) \n", argv[i]);

                strncat(buffer, argv[i], totalLen);
                totalLen -= len;

                // special case - delay
                if (strcmp(argv[i], "-delay") == 0) {
                    // process seconds
                    if ((i + 1) < argc && isNumber(argv[i+1]) ) {
                        strncat(buffer, " ", totalLen);
                        totalLen -= 1;

                        i++;
                        len = (int)strlen(argv[i]);

                        printf("Add (%s) \n", argv[i]);
                        strncat(buffer, argv[i], totalLen);
                        totalLen -= len;
                    }
                }

                found = true;
                break;
            }
        }

        if (!found) {
            printf("Error in (%s) \n", argv[i]);
            return false;
        }
    }

    return true;
}

int main(int argc, char *argv[])
{
    EFI_STATUS status;
    CHAR16 variable[512];

    char buffer[512];

    if (parseArgv(argc, argv, sizeof(buffer), buffer) == false) {
        return -1;
    }

    printf("ASCII   variable (%s) \n", buffer);

    // We must use "%a" for ascii string.
    UnicodeSPrintAsciiFormat(variable, sizeof(variable), "%a", buffer);

    Print(L"UNICODE variable (%s) \n", variable);

    status = gRT->SetVariable(
        (CHAR16*)L"ShellOpt",
        &gShellVariableGuid,
        EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
        sizeof(variable),
        (VOID*)variable
    );

    printf("SetVariable status(%02d) \n", status);

    return 0;
}
