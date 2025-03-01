#include "storage.h"

#include "BSP_W25Qx.h"
#include "easyflash.h"

#ifdef STORAGE_USE_CLI
#include "FreeRTOS_CLI.h"
#include "console_colors.h"
#include "s2j.h"

static BaseType_t prvEnvCommand(char* pcWriteBuffer,
                                size_t xWriteBufferLen,
                                const char* pcCommandString);
static CLI_Command_Definition_t xEnv = {
    .pcCommand = "env",
    // .pcHelpString = "\n\renv: \n\rUsage: env <command> [<args>]\n\n\rCommands:\n\r\tset <name> <value> \t\tSet an environment variable.\n\r\tget <name>\t\t\tget an environment variable.\n\r",
    .pcHelpString = " \n\renv: \n\r  Manage environment variable.\n\r",
    .pxCommandInterpreter = prvEnvCommand,
    .cExpectedNumberOfParameters = -1  // 允许输入2或者3个参数
};

#endif

void Storage_Init() {
    BSP_W25Qx_Init();
    easyflash_init();

#ifdef STORAGE_USE_CLI
    FreeRTOS_CLIRegisterCommand(&xEnv);
#endif
}

#ifdef STORAGE_USE_CLI
static BaseType_t prvEnvCommand(char* pcWriteBuffer,
                                size_t xWriteBufferLen,
                                const char* pcCommandString) {
    const char* pcParameter;
    BaseType_t xParameterStringLength, xReturn;
    // static UBaseType_t uxParameterNumber = 0;

    (void)pcCommandString;
    (void)xWriteBufferLen;
    configASSERT(pcWriteBuffer);
    char* pDiv;
    EfErrCode ret;

    bzero(pcWriteBuffer, xWriteBufferLen);

    pcParameter = FreeRTOS_CLIGetParameter(pcCommandString, 1, &xParameterStringLength);
    if (pcParameter == NULL)
        goto on_err_use;

    if (strncmp((char*)pcParameter, "set", xParameterStringLength) == 0) {
        const char* pName = FreeRTOS_CLIGetParameter(pcCommandString, 2, &xParameterStringLength);
        pDiv = (char*)(pName + xParameterStringLength);
        const char* pValue = FreeRTOS_CLIGetParameter(pcCommandString, 3, &xParameterStringLength);
        *pDiv = '\0';
        pDiv = (char*)(pValue + xParameterStringLength);
        *pDiv = '\0';

        if (pName == NULL || pValue == NULL)
            goto on_err_use;

        ret = ef_set_env(pName, pValue);

        goto on_ret;
    } else if (strncmp((char*)pcParameter, "get", xParameterStringLength) == 0) {
        const char* pName = FreeRTOS_CLIGetParameter(pcCommandString, 2, &xParameterStringLength);
        pDiv = (char*)(pName + xParameterStringLength);
        *pDiv = '\0';

        const char* pValue;
        if (pName == NULL)
            goto on_err_use;

        pValue = ef_get_env(pName);

        if (pValue != NULL) {
            snprintf(pcWriteBuffer, xWriteBufferLen, GREEN "%s=%s\n\r" NC, pName, pValue);
        } else {
            snprintf(pcWriteBuffer, xWriteBufferLen, RED "Faild!\n\r" NC);
        }
    } else if (strncmp((char*)pcParameter, "rm", xParameterStringLength) == 0) {
        const char* pName = FreeRTOS_CLIGetParameter(pcCommandString, 2, &xParameterStringLength);
        pDiv = (char*)(pName + xParameterStringLength);
        *pDiv = '\0';

        const char* pValue;
        if (pName == NULL)
            goto on_err_use;

        ret = ef_del_env(pName);

        goto on_ret;
    } else if (strncmp((char*)pcParameter, "reset", xParameterStringLength) == 0) {
        ret = ef_env_set_default();
        goto on_ret;
    } else if (strncmp((char*)pcParameter, "list", xParameterStringLength) == 0) {
        // USB_Printf(YELLOW);
        ef_print_env();
        // USB_Printf(NC);
    } else
        goto on_err_use;

    return pdFALSE;

    static const char* msg[] = {
        "Success!",
        "Erase Faild!",
        "Read Faild!",
        "Write Faild!",
        "Name Error!",
        "Name exist!",
        "Env FULL!",
        "Init Failed",
    };

on_ret:
    switch (ret) {
    case EF_NO_ERR:
        snprintf(pcWriteBuffer, xWriteBufferLen, GREEN "%s\n\r" NC, msg[ret]);
        break;

    default:
        snprintf(pcWriteBuffer, xWriteBufferLen, RED "%s\n\r" NC, msg[ret]);
        break;
    }

    return pdFALSE;

on_err_use:
    snprintf(pcWriteBuffer, xWriteBufferLen, RED "Wrong Usage!\n\r" NC);
    return pdFALSE;
}

#endif