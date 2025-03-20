/*
 * This file is part of the EasyFlash Library.
 *
 * Copyright (c) 2015-2019, Armink, <armink.ztl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: Portable interface for each platform.
 * Created on: 2015-01-16
 */

#include <easyflash.h>
#include <stdarg.h>
#include <stdio.h>

#include "main.h"
//
#include <FreeRTOS.h>

#include "justfw_cfg.h"
#include "semphr.h"

static xSemaphoreHandle xEfPortMutex;

#ifdef USE_BOARD_D
#include "BSP_W25Qx.h"
#endif

#ifdef USE_BOARD_C

// FLASH 扇区的起始地址
#define ADDR_FLASH_SECTOR_0 ((uint32_t)0x08000000)   // 扇区0起始地址, 16 Kbytes
#define ADDR_FLASH_SECTOR_1 ((uint32_t)0x08004000)   // 扇区1起始地址, 16 Kbytes
#define ADDR_FLASH_SECTOR_2 ((uint32_t)0x08008000)   // 扇区2起始地址, 16 Kbytes
#define ADDR_FLASH_SECTOR_3 ((uint32_t)0x0800C000)   // 扇区3起始地址, 16 Kbytes
#define ADDR_FLASH_SECTOR_4 ((uint32_t)0x08010000)   // 扇区4起始地址, 64 Kbytes
#define ADDR_FLASH_SECTOR_5 ((uint32_t)0x08020000)   // 扇区5起始地址, 128 Kbytes
#define ADDR_FLASH_SECTOR_6 ((uint32_t)0x08040000)   // 扇区6起始地址, 128 Kbytes
#define ADDR_FLASH_SECTOR_7 ((uint32_t)0x08060000)   // 扇区7起始地址, 128 Kbytes
#define ADDR_FLASH_SECTOR_8 ((uint32_t)0x08080000)   // 扇区8起始地址, 128 Kbytes
#define ADDR_FLASH_SECTOR_9 ((uint32_t)0x080A0000)   // 扇区9起始地址, 128 Kbytes
#define ADDR_FLASH_SECTOR_10 ((uint32_t)0x080C0000)  // 扇区10起始地址,128 Kbytes
#define ADDR_FLASH_SECTOR_11 ((uint32_t)0x080E0000)  // 扇区11起始地址,128 Kbytes

static uint32_t stm32_get_sector(uint32_t addr) {
    if (addr < ADDR_FLASH_SECTOR_1)
        return FLASH_SECTOR_0;
    else if (addr < ADDR_FLASH_SECTOR_2)
        return FLASH_SECTOR_1;
    else if (addr < ADDR_FLASH_SECTOR_3)
        return FLASH_SECTOR_2;
    else if (addr < ADDR_FLASH_SECTOR_4)
        return FLASH_SECTOR_3;
    else if (addr < ADDR_FLASH_SECTOR_5)
        return FLASH_SECTOR_4;
    else if (addr < ADDR_FLASH_SECTOR_6)
        return FLASH_SECTOR_5;
    else if (addr < ADDR_FLASH_SECTOR_7)
        return FLASH_SECTOR_6;
    else if (addr < ADDR_FLASH_SECTOR_8)
        return FLASH_SECTOR_7;
    else if (addr < ADDR_FLASH_SECTOR_9)
        return FLASH_SECTOR_8;
    else if (addr < ADDR_FLASH_SECTOR_10)
        return FLASH_SECTOR_9;
    else if (addr < ADDR_FLASH_SECTOR_11)
        return FLASH_SECTOR_10;
    else
        return FLASH_SECTOR_11;
}

static uint32_t stm32_get_sector_size(uint32_t sector) {
    EF_ASSERT(IS_FLASH_SECTOR(sector));

    switch (sector) {
    case FLASH_SECTOR_0:
        return 16 * 1024;
    case FLASH_SECTOR_1:
        return 16 * 1024;
    case FLASH_SECTOR_2:
        return 16 * 1024;
    case FLASH_SECTOR_3:
        return 16 * 1024;
    case FLASH_SECTOR_4:
        return 64 * 1024;
    case FLASH_SECTOR_5:
        return 128 * 1024;
    case FLASH_SECTOR_6:
        return 128 * 1024;
    case FLASH_SECTOR_7:
        return 128 * 1024;
    case FLASH_SECTOR_8:
        return 128 * 1024;
    case FLASH_SECTOR_9:
        return 128 * 1024;
    case FLASH_SECTOR_10:
        return 128 * 1024;
    case FLASH_SECTOR_11:
        return 128 * 1024;
    }

    return 0;
}

static uint32_t STMFLASH_ReadWord(uint32_t faddr) {
    return *(__IO uint32_t *)faddr;
}

#endif

// static xSemaphoreHandle ef_mutex = NULL;

/* default environment variables set for user */
static const ef_env default_env_set[] = {
    {(char *)"test", "HelloWorld", 0}};

/**
 * Flash port for hardware initialize.
 *
 * @param default_env default ENV set for user
 * @param default_env_size default ENV size
 *
 * @return result
 */
EfErrCode ef_port_init(ef_env const **default_env, size_t *default_env_size) {
#ifdef USE_BOARD_C
    (void)STMFLASH_ReadWord;
#endif
    EfErrCode result = EF_NO_ERR;
    xEfPortMutex = xSemaphoreCreateMutex();

    *default_env = default_env_set;
    *default_env_size = sizeof(default_env_set) / sizeof(default_env_set[0]);

    return result;
}

/**
 * Read data from flash.
 * @note This operation's units is word.
 *
 * @param addr flash address
 * @param buf buffer to store read data
 * @param size read bytes size
 *
 * @return result
 */
EfErrCode ef_port_read(uint32_t addr, uint32_t *buf, size_t size) {
    EfErrCode result = EF_NO_ERR;

/* You can add your code under here. */
#ifdef USE_BOARD_D
    result = BSP_W25Qx_Read((uint8_t *)buf, addr, size);
#endif

#ifdef USE_BOARD_C

    uint8_t *buf_8 = (uint8_t *)buf;
    size_t i;

    for (i = 0; i < size; i++, addr++, buf_8++) {
        *buf_8 = *(uint8_t *)addr;
    }

    return result;
#endif

    return result;
}

/**
 * Erase data on flash.
 * @note This operation is irreversible.
 * @note This operation's units is different which on many chips.
 *
 * @param addr flash address
 * @param size erase bytes size
 *
 * @return result
 */
EfErrCode ef_port_erase(uint32_t addr, size_t size) {
    EfErrCode result = EF_NO_ERR;

    /* make sure the start address is a multiple of EF_ERASE_MIN_SIZE */
    EF_ASSERT(addr % EF_ERASE_MIN_SIZE == 0);

    /* You can add your code under here. */

#ifdef USE_BOARD_D
    for (uint32_t i = 0; i < ((size - 1) / EF_ERASE_MIN_SIZE + 1); i++)
        result |= BSP_W25Qx_Erase_Block(addr + i * EF_ERASE_MIN_SIZE);
#endif

#ifdef USE_BOARD_C
    FLASH_EraseInitTypeDef FlashEraseInit;
    uint32_t SectorError = 0;

    size_t erased_size = 0;
    HAL_FLASH_Unlock();
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
    FlashEraseInit.TypeErase = FLASH_TYPEERASE_SECTORS;  // 擦除类型，扇区擦除

    FlashEraseInit.NbSectors = 1;                         // 一次只擦除一个扇区
    FlashEraseInit.VoltageRange = FLASH_VOLTAGE_RANGE_3;  // 电压范围，VCC=2.7~3.6V之间!!
    /* it will stop when erased size is greater than setting size */
    while (erased_size < size) {
        FlashEraseInit.Sector = stm32_get_sector(addr + erased_size);  // 要擦除的扇区
        if (HAL_FLASHEx_Erase(&FlashEraseInit, &SectorError) != HAL_OK) {
            result = EF_ERASE_ERR;
            break;
        }
        erased_size += stm32_get_sector_size(FlashEraseInit.Sector);
    }
    HAL_FLASH_Lock();
#endif

    return result;
}
/**
 * Write data to flash.
 * @note This operation's units is word.
 * @note This operation must after erase. @see flash_erase.
 *
 * @param addr flash address
 * @param buf the write data buffer
 * @param size write bytes size
 *
 * @return result
 */
EfErrCode ef_port_write(uint32_t addr, const uint32_t *buf, size_t size) {
    EfErrCode result = EF_NO_ERR;

    /* You can add your code under here. */

#ifdef USE_BOARD_D
    result = BSP_W25Qx_Write((uint8_t *)buf, addr, size);
#endif

#ifdef USE_BOARD_C
    size_t i;
    uint32_t read_data;
    uint8_t *buf_8 = (uint8_t *)buf;

    HAL_FLASH_Unlock();
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
    for (i = 0; i < size; i++, buf_8++, addr++) {
        /* write data */
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, addr, *buf_8);
        read_data = *(uint8_t *)addr;
        /* check data */
        if (read_data != *buf_8) {
            result = EF_WRITE_ERR;
            break;
        }
    }
    HAL_FLASH_Lock();
#endif

    return result;
}

/**
 * lock the ENV ram cache
 */
void ef_port_env_lock(void) {
    /* You can add your code under here. */
    xSemaphoreTake(xEfPortMutex, portMAX_DELAY);
}

/**
 * unlock the ENV ram cache
 */
void ef_port_env_unlock(void) {
    /* You can add your code under here. */
    xSemaphoreGive(xEfPortMutex);
}

/**
 * This function is print flash debug info.
 *
 * @param file the file which has call this function
 * @param line the line number which has call this function
 * @param format output format
 * @param ... args
 *
 */
void ef_log_debug(const char *file, const long line, const char *format, ...) {
#ifdef PRINT_DEBUG

    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);

    /* You can add your code under here. */
    vprintf(format, args);

    va_end(args);

#endif
}

/**
 * This function is print flash routine info.
 *
 * @param format output format
 * @param ... args
 */
void ef_log_info(const char *format, ...) {
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);

    /* You can add your code under here. */
    vprintf(format, args);

    va_end(args);
}
/**
 * This function is print flash non-package info.
 *
 * @param format output format
 * @param ... args
 */
void ef_print(const char *format, ...) {
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);

    /* You can add your code under here. */
    vprintf(format, args);

    va_end(args);
}
