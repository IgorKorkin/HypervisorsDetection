/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Eric W. Biederman
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef CPU_X86_CACHE
#define CPU_X86_CACHE

static inline unsigned long read_cr0(void)
{
        unsigned long cr0;
		__asm
		{
			PUSH  EAX

			MOV   EAX, CR0
			MOV   cr0, EAX

			POP   EAX
		}     
        return cr0;
}

static inline void write_cr0(unsigned long cr0)
{
		__asm
		{
			PUSH  EAX

			MOV   EAX, cr0
			MOV   CR0, EAX

			POP   EAX
		}
}

static inline void wbinvd(void)
{
		__asm
		{
			WBINVD
		}
}

static inline void invd(void)
{
		__asm
		{
			INVD
		}
}

/* The following functions require the always_inline due to AMD
 * function STOP_CAR_AND_CPU that disables cache as
 * ram, the cache as ram stack can no longer be used. Called
 * functions must be inlined to avoid stack usage. Also, the
 * compiler must keep local variables register based and not
 * allocated them from the stack. With gcc 4.5.0, some functions
 * declared as inline are not being inlined. This patch forces
 * these functions to always be inlined by adding the qualifier
 * __attribute__((always_inline)) to their declaration.
 */

static inline void enable_cache(void)
{
        unsigned long cr0 = 0;
        cr0 = read_cr0();
        cr0 &= 0x9fffffff;
        write_cr0(cr0);
}

static inline void disable_cache(void)
{
        /* Disable and write back the cache */
        unsigned long cr0 = 0;
        cr0 = read_cr0();
        cr0 |= 0x40000000;
        wbinvd();
        write_cr0(cr0);
        wbinvd();		
}

#endif /* CPU_X86_CACHE */