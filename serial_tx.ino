#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif


struct fifo_big {
  uint16_t idx_w;
  uint16_t idx_r;
  uint16_t count;
  uint8_t buff[256];
};

struct fifo_small {
  uint16_t idx_w;
  uint16_t idx_r;
  uint16_t count;
  uint8_t buff[32];
};

volatile fifo_big txfifo0 = { 0, 0, 0, { 0 } }; // transmit buffer for PC
volatile fifo_small txfifo1 = { 0, 0, 0, { 0 } }; // transmit buffer for LCD
volatile fifo_small txfifo3 = { 0, 0, 0, { 0 } }; // transmit buffer for servo


//-----------------UART 0 support routines (Serial transmit)------------------

// tx interrupt handler
SIGNAL(SIG_USART0_DATA)
{
  uint16_t n, i;

  n = txfifo0.count;
  if(n) {
    txfifo0.count = --n;
    i = txfifo0.idx_r;
    UDR0 = txfifo0.buff[i++];
    if(i >= sizeof(txfifo0.buff))
      i = 0;
    txfifo0.idx_r = i;
  }
  if(n == 0)
    cbi(UCSR0B, UDRIE0);
}

// tx put routine
void uart0_put (uint8_t d) {
  uint16_t i;
  i = txfifo0.idx_w;
  while(txfifo0.count >= sizeof(txfifo0.buff));
  txfifo0.buff[i++] = d;
  uint8_t oldSREG = SREG;
  cli();
  txfifo0.count++;
  sbi(UCSR0B, UDRIE0);
  SREG = oldSREG;
  if(i >= sizeof(txfifo0.buff))
    i = 0;
  txfifo0.idx_w = i;
}

// print a unsigned 32-bit value
void printValue0_U32(uint32_t msec) {
  uint8_t pr = 0;
  uint8_t d = 0;
  while (msec > 999999999) {
    d++;
    msec -= 1000000000;
  }
  if (pr || d) {
    uart0_put(d+0x30);
    pr = 1;
  }

  d = 0;
  while (msec > 99999999) {
    d++;
    msec -= 100000000;
  }
  if (pr || d) {
    uart0_put(d+0x30);
    pr = 1;
  }

  d = 0;
  while (msec > 9999999) {
    d++;
    msec -= 10000000;
  }
  if (pr || d) {
    uart0_put(d+0x30);
    pr = 1;
  }

  d = 0;
  while (msec > 999999) {
    d++;
    msec -= 1000000;
  }
  if (pr || d) {
    uart0_put(d+0x30);
    pr = 1;
  }

  d = 0;
  while (msec > 99999) {
    d++;
    msec -= 100000;
  }
  if (pr || d) {
    uart0_put(d+0x30);
    pr = 1;
  }

  d = 0;
  while (msec > 9999) {
    d++;
    msec -= 10000;
  }
  if (pr || d) {
    uart0_put(d+0x30);
    pr = 1;
  }

  d = 0;
  while (msec > 999) {
    d++;
    msec -= 1000;
  }
  if (pr || d) {
    uart0_put(d+0x30);
    pr = 1;
  }
  
  d = 0;
  while (msec > 99) {
    d++;
    msec -= 100;
  }
  if (pr || d) {
    uart0_put(d+0x30);
    pr = 1;
  }

  d = 0;
  while (msec > 9) {
    d++;
    msec -= 10;
  }
  if (pr || d)
    uart0_put(d+0x30);

  d = msec;
  uart0_put(d+0x30);
}

// print new-line (CR,LF)
void printNewline0() {
  uart0_put(0x0d);
  uart0_put(0x0a);
}

// print a string
void printString0(char *line) {
  while (*line)
    uart0_put(*line++);
}

// print a string + new-line
void printlnString0(char *line) {
  while (*line)
    uart0_put(*line++);
  printNewline0();
}



//-----------------UART 1 support routines (Serial1 transmit)------------------

SIGNAL(SIG_USART1_DATA)
{
  uint16_t n, i;

  n = txfifo1.count;
  if(n) {
    txfifo1.count = --n;
    i = txfifo1.idx_r;
    UDR1 = txfifo1.buff[i++];
    if(i >= sizeof(txfifo1.buff))
      i = 0;
    txfifo1.idx_r = i;
  }
  if(n == 0)
    cbi(UCSR1B, UDRIE1);
}

void uart1_put (uint8_t d) {
  uint16_t i;
  i = txfifo1.idx_w;
  while(txfifo1.count >= sizeof(txfifo1.buff));
  txfifo1.buff[i++] = d;
  uint8_t oldSREG = SREG;
  cli();
  txfifo1.count++;
  sbi(UCSR1B, UDRIE1);
  SREG = oldSREG;
  if(i >= sizeof(txfifo1.buff))
    i = 0;
  txfifo1.idx_w = i;
}

// print a string
void printString1(char *line) {
  while (*line)
    uart1_put(*line++);
}


//-----------------UART 3 support routines (Serial3 transmit)------------------

SIGNAL(SIG_USART3_DATA)
{
  uint16_t n, i;

  n = txfifo3.count;
  if(n) {
    txfifo3.count = --n;
    i = txfifo3.idx_r;
    UDR3 = txfifo3.buff[i++];
    if(i >= sizeof(txfifo3.buff))
      i = 0;
    txfifo3.idx_r = i;
  }
  if(n == 0)
    cbi(UCSR3B, UDRIE3);
}

void uart3_put (uint8_t d) {
  uint16_t i;
  i = txfifo3.idx_w;
  while(txfifo3.count >= sizeof(txfifo3.buff));
  txfifo3.buff[i++] = d;
  uint8_t oldSREG = SREG;
  cli();
  txfifo3.count++;
  sbi(UCSR3B, UDRIE3);
  SREG = oldSREG;
  if(i >= sizeof(txfifo3.buff))
    i = 0;
  txfifo3.idx_w = i;
}

