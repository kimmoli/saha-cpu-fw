#include <stdlib.h>
#include <string.h>

#include "ch.h"
#include "hal.h"
#include "helpers.h"
#include "chprintf.h"
#include "shell.h"
#include "shellcommands.h"

#include "i2c.h"

int main(void)
{
    halInit();
    chSysInit();

    sdStart(&SD6, NULL);  /* Serial console in USART6, 115200 */

    consoleStream = (BaseSequentialStream *) &SD6;

    PRINT("\n\r");
    PRINT("\n\rSAHA CPU\n\r");
    PRINT("--------\n\r");
    PRINT("\n\r");

    initI2C();

    PRINT(" - Initialisation complete\n\r");

    /* Start threads */
    PRINT(" - Threads started\n\r");

    PRINT("\n\r");

    /* Everything is initialised, turh red led off */
    palClearLine(LINE_LEDRED);

    shellInit();

    chThdCreateFromHeap(NULL, SHELL_WA_SIZE, "shell", NORMALPRIO + 1, shellThread, (void *)&shell_cfg_uart);

    while (true)
    {
        chThdSleepMilliseconds(500);
        palToggleLine(LINE_LEDGRN);
    }
}
