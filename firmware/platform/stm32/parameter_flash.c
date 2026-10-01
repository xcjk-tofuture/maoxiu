#include "parameter_flash.h"
#include "stm32f4xx_hal.h"
#include <string.h>
static const uint32_t addresses[2] = {0x08040000u, 0x08060000u};
static int bounds(unsigned slot, unsigned offset, size_t n) {
    return slot < 2 && offset <= PJ_RECORD_BYTES && n <= PJ_RECORD_BYTES - offset;
}
static int read_bytes(void *ctx, unsigned slot, unsigned offset, uint8_t *b, size_t n) {
    (void)ctx;
    if (!bounds(slot, offset, n))
        return -1;
    memcpy(b, (const void *)(addresses[slot] + offset), n);
    return 0;
}
static int erase_slot(void *ctx, unsigned slot) {
    (void)ctx;
    if (slot > 1)
        return -1;
    FLASH_EraseInitTypeDef e = {0};
    uint32_t failed;
    e.TypeErase = FLASH_TYPEERASE_SECTORS;
    e.Sector = slot ? FLASH_SECTOR_7 : FLASH_SECTOR_6;
    e.NbSectors = 1;
    e.VoltageRange = FLASH_VOLTAGE_RANGE_3;
    if (HAL_FLASH_Unlock() != HAL_OK)
        return -1;
    HAL_StatusTypeDef r = HAL_FLASHEx_Erase(&e, &failed);
    HAL_FLASH_Lock();
    return r == HAL_OK ? 0 : -1;
}
static int program_bytes(void *ctx, unsigned slot, unsigned offset, const uint8_t *b, size_t n) {
    (void)ctx;
    if (!bounds(slot, offset, n) || HAL_FLASH_Unlock() != HAL_OK)
        return -1;
    for (size_t i = 0; i < n; i++) {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, addresses[slot] + offset + i, b[i]) !=
            HAL_OK) {
            HAL_FLASH_Lock();
            return -1;
        }
    }
    HAL_FLASH_Lock();
    /* Data cache may contain the pre-erase value of this flash address. */
    __HAL_FLASH_DATA_CACHE_DISABLE();
    __HAL_FLASH_DATA_CACHE_RESET();
    __HAL_FLASH_DATA_CACHE_ENABLE();
    return 0;
}
const pj_io_t *parameter_flash_io(void) {
    static const pj_io_t io = {NULL, read_bytes, erase_slot, program_bytes};
    return &io;
}
