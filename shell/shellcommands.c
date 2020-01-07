#include "hal.h"
#include "shell.h"
#include "shellcommands.h"

char uartShellHistoryBuffer[SHELL_MAX_HIST_BUFF];

const ShellCommand commands[] =
{
    { "diic",    cmd_diic },
    { "piic",    cmd_piic },
    { "probe",   cmd_probe },
    { "reboot",  cmd_reboot },
    { "dmb",     cmd_dmb },
    { "dmw",     cmd_dmw },
    { "disp",    cmd_disp },
    { "step",    cmd_step },
    { "in",      cmd_in },
    { "rele",    cmd_rele },
    { "ee",      cmd_eeprom },
    {NULL, NULL}
};

const ShellConfig shell_cfg_uart =
{
    (BaseSequentialStream *)&SD6,
    commands,
    uartShellHistoryBuffer,
    SHELL_MAX_HIST_BUFF
};
