/*
 * 1-Wire implementation for MSP430
 *
 * @author: David Siroky <siroky@dasir.cz>
 * @license: MIT
 */

#ifndef __DELAY_H
#define __DELAY_H

#define CYCLES_PER_US 8L // depends on the CPU speed
#define CYCLES_PER_MS (CYCLES_PER_US * 1000L)

#define DELAY_US(x) __delay_cycles((x * CYCLES_PER_US))
#define DELAY_MS(x) __delay_cycles((x * CYCLES_PER_MS))

#endif
