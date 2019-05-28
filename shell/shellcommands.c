#include "hal.h"
#include "shell.h"
#include "shellcommands.h"

char uartShellHistoryBuffer[SHELL_MAX_HIST_BUFF];

const ShellCommand commands[] =
{
    { "diic",    cmd_diic },
    { "piic",    cmd_piic },
    { "probe",    cmd_probe },
    { "reboot",  cmd_reboot },
    {NULL, NULL}
};

const ShellConfig shell_cfg_uart =
{
    (BaseSequentialStream *)&SD6,
    commands,
    uartShellHistoryBuffer,
    SHELL_MAX_HIST_BUFF
};
