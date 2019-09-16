#ifndef _SHELLCOMMANDS_H
#define _SHELLCOMMANDS_H

#include "hal.h"
#include "shell.h"

#define SHELL_WA_SIZE   THD_WORKING_AREA_SIZE(2048)

#if 0
extern void cmd_status(BaseSequentialStream *chp, int argc, char *argv[]);
#endif
extern void cmd_diic(BaseSequentialStream *chp, int argc, char *argv[]);
extern void cmd_piic(BaseSequentialStream *chp, int argc, char *argv[]);
extern void cmd_probe(BaseSequentialStream *chp, int argc, char *argv[]);
extern void cmd_reboot(BaseSequentialStream *chp, int argc, char *argv[]);
extern void cmd_dmb(BaseSequentialStream *chp, int argc, char *argv[]);
extern void cmd_dmw(BaseSequentialStream *chp, int argc, char *argv[]);
extern void cmd_disp(BaseSequentialStream *chp, int argc, char *argv[]);
extern void cmd_step(BaseSequentialStream *chp, int argc, char *argv[]);
extern void cmd_in(BaseSequentialStream *chp, int argc, char *argv[]);

extern const ShellCommand commands[];
extern const ShellConfig shell_cfg_uart;

#endif // _SHELLCOMMANDS_H
