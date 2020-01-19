#include <stdlib.h>
#include <string.h>
#include "hal.h"
#include "chprintf.h"
#include "shellcommands.h"
#include "rele.h"

void cmd_rele(BaseSequentialStream *chp, int argc, char *argv[])
{
    if (argc == 2)
    {
        uint8_t relay = strtol(argv[0], NULL, 10) -1;
        if (relay > 3)
        {
            chprintf(chp, "relay 1...4\n\r");
            return;
        }
        bool value = strtol(argv[0], NULL, 10) == 1;

        setRelay(RELAY_1 << relay, value);
        return;
    }

    uint16_t relays = getRelay();
    uint16_t inputs = getRelayInputs();

    chprintf(chp, "Relays %d %d %d %d\n\r",
        ((relays & RELAY_1) > 0),
        ((relays & RELAY_2) > 0),
        ((relays & RELAY_3) > 0),
        ((relays & RELAY_4) > 0));
    chprintf(chp, "Inputs %d %d %d %d\n\r",
        ((inputs & AUX_IN_1) > 0),
        ((inputs & AUX_IN_2) > 0),
        ((inputs & AUX_IN_3) > 0),
        ((inputs & AUX_IN_4) > 0));
}

