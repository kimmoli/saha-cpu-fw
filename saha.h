#ifndef SAHA_H
#define SAHA_H

#define KEY_MAN_UP   0x0001
#define KEY_MAN_DOWN 0x0002
#define KEY_PGM_UP   0x0004
#define KEY_PGM_DOWN 0x0008

#define KEY_I        0x0020
#define KEY_II       0x0040
#define KEY_O        0x0080

#define KEY_STOP     0x8000

#define MODE_PGM     0x10
#define MODE_SEL     0x20
#define MODE_RUN     0x40
#define MODE_STOP    0x80

#define SLOTS        5

extern void initSaha(void);

#endif // SAHA_H

