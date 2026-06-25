/**
    @file globals.c
    @brief Driver board control source file

    @version
*/

#include <stdint.h>

uint32_t chip_id = 0;
uint8_t newMsg;
uint32_t gp_buffer[8192] = {0};

