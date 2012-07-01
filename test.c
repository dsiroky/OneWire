/* 
 * test for TI LaunchPad
 * @author: David Siroky (siroky@dasir.cz)
 * @license: public domain
 */

#include <msp430.h>
#include <stdio.h>
#include <stdint.h>
 
#include "onewire.h"
#include "delay.h"
 
/***************************************************************/
 
int putchar(int c)
{
  if (c == '\n') putchar('\r');
  while (!(IFG2&UCA0TXIFG));
  UCA0TXBUF = c;
  return 0;
}
 
/***************************************************************/
 
void uart_setup()
{
  P1SEL = BIT1 + BIT2;
  P1SEL2 = BIT1 + BIT2;
  P1DIR &= ~ BIT1;
  P1DIR |= BIT2;
  UCA0CTL1 |= UCSSEL_2;                     // SMCLK
  UCA0BR0 = 0x41;                            // 8MHz 9600
  UCA0BR1 = 0x03;                              // 8MHz 9600
  UCA0MCTL = UCBRS0;                        // Modulation UCBRSx = 1
  UCA0CTL1 &= ~UCSWRST;
}
 
/***************************************************************/
 
void search(onewire_t *ow, uint8_t *id, int depth, int reset)
{
  int i, b1, b2;
 
  if (depth == 64)
  {
    // we have all 64 bit in this search branch
    printf("found: ");
    for (i = 0; i < 8; i++) printf("%02x", id[i]);
    printf("\n");
    return;
  }
 
  if (reset)
  {
    if (onewire_reset(ow) != 0) { printf("reset failed\n"); return; }
    onewire_write_byte(ow, 0xF0); // search ROM command
 
    // send currently recognized bits
    for (i = 0; i < depth; i++)
    {
      b1 = onewire_read_bit(ow);
      b2 = onewire_read_bit(ow);
      onewire_write_bit(ow, id[i / 8] & (1 << (i % 8)));
    }
  }
 
  // check another bit
  b1 = onewire_read_bit(ow);
  b2 = onewire_read_bit(ow);
  if (b1 && b2) return; // no response to search
  if (!b1 && !b2) // two devices with different bits on this position
  {
    // check devices with this bit = 0
    onewire_write_bit(ow, 0);
    id[depth / 8] &= ~(1 << (depth % 8));
    search(ow, id, depth + 1, 0);
    // check devices with this bit = 1
    id[depth / 8] |= 1 << (depth % 8);
    search(ow, id, depth + 1, 1); // different branch, reset must be issued
  } else if (b1) {
    // devices have 1 on this position
    onewire_write_bit(ow, 1);
    id[depth / 8] |= 1 << (depth % 8);
    search(ow, id, depth + 1, 0);
  } else if (b2) {
    // devices have 0 on this position
    onewire_write_bit(ow, 0);
    id[depth / 8] &= ~(1 << (depth % 8));
    search(ow, id, depth + 1, 0);
  }
}
 
/***************************************************************/
 
int main()
{
  onewire_t ow;
  uint8_t id[8];
 
  WDTCTL = WDTPW + WDTHOLD; //Stop watchdog timer
  BCSCTL1 = CALBC1_8MHZ;
  DCOCTL = CALDCO_8MHZ;
  uart_setup();
 
  ow.port_out = &P1OUT;
  ow.port_in = &P1IN;
  ow.port_ren = &P1REN;
  ow.port_dir = &P1DIR;
  ow.pin = BIT7;
 
  printf("start\n");
  search(&ow, id, 0, 1);
  printf("done\n");
 
  _BIS_SR(LPM0_bits + GIE);
  return 0;
}
