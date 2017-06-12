/*
 * PRU1 Firmware for BeagleLogic
 *
 * Copyright (C) 2014-17 Kumar Abhishek <abhishek@theembeddedkitchen.net>
 *
 * This file is a part of the BeagleLogic project
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <stdint.h>
#include "resource_table_1.h"

extern void asm_main();

void main()
{
        asm_main();
}
