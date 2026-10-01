#ifndef PARAMETER_FLASH_H
#define PARAMETER_FLASH_H
#include "param_journal.h"
/* STM32 internal flash sectors 6/7 are reserved by linker. Initialization reads
 * only. Saving stalls same-bank instruction fetch: control must be in acknowledged
 * maintenance stop. Erases are explicit, never automatic on parameter writes. */
const pj_io_t *parameter_flash_io(void);
#endif
