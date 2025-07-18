#ifndef ATMEGA328P_H        // Prevent multiple inclusion of this header file
#define ATMEGA328P_H

#include <stdint.h>         // Include standard integer types

// Configuration Structures

typedef struct {
    uint8_t ddr;   // Data Direction Register address or value
    uint8_t port;  // Port Data Register address or value
    uint8_t pin;   // Pin Input Register address or value
} GPIO_TypeDef;

typedef struct {
    uint8_t tccra; // Timer/Counter Control Register A
    uint8_t tccrb; // Timer/Counter Control Register B
    uint8_t tcnt;  // Timer/Counter Value
    uint8_t ocra;  // Output Compare Register A
    uint8_t ocrb;  // Output Compare Register B
    uint8_t timsk; // Timer Interrupt Mask Register
    uint8_t tifr;  // Timer Interrupt Flag Register
} TIMER8_TypeDef;

typedef struct {
    uint8_t tccra;     // Timer/Counter Control Register A
    uint8_t tccrb;     // Timer/Counter Control Register B
    uint8_t tccrc;     // Timer/Counter Control Register C
    uint16_t tcnt;     // Timer/Counter Value (16-bit)
    uint16_t ocra;     // Output Compare Register A (16-bit)
    uint16_t ocrb;     // Output Compare Register B (16-bit)
    uint16_t icr;      // Input Capture Register (16-bit)
    uint8_t timsk;     // Timer Interrupt Mask Register
    uint8_t tifr;      // Timer Interrupt Flag Register
} TIMER16_TypeDef;

typedef struct {
    uint8_t admux;     // ADC Multiplexer Selection Register
    uint8_t adcsra;    // ADC Control and Status Register A
    uint8_t adcsrb;    // ADC Control and Status Register B
    uint16_t adc;      // ADC Data Register (16-bit)
} ADC_TypeDef;

typedef struct {
    uint8_t udr;       // USART I/O Data Register
    uint8_t ucsra;     // USART Control and Status Register A
    uint8_t ucsrb;     // USART Control and Status Register B
    uint8_t ucsrc;     // USART Control and Status Register C
    uint8_t ubrrl;     // USART Baud Rate Register Low
    uint8_t ubrrh;     // USART Baud Rate Register High
} USART_TypeDef;

typedef struct {
    uint8_t spcr;      // SPI Control Register
    uint8_t spsr;      // SPI Status Register
    uint8_t spdr;      // SPI Data Register
} SPI_TypeDef;

typedef struct {
    uint8_t twbr;      // TWI Bit Rate Register
    uint8_t twsr;      // TWI Status Register
    uint8_t twar;      // TWI (Slave) Address Register
    uint8_t twdr;      // TWI Data Register
    uint8_t twcr;      // TWI Control Register
    uint8_t twamr;     // TWI (Slave) Address Mask Register
} TWI_TypeDef;

#define HIGH            1
#define LOW             0

#define ENABLE          HIGH
#define DISABLE         LOW

#define F_CPU           8000000UL

#define EVEN            1
#define ODD             2

// Base addresses for peripherals (from ATmega328P datasheet)
#define GPIOB_BASE   0x23   // Base address for GPIO Port B registers
#define GPIOC_BASE   0x26   // Base address for GPIO Port C registers
#define GPIOD_BASE   0x29   // Base address for GPIO Port D registers

#define TIMER0_BASE  0x44   // Base address for Timer/Counter0 registers
#define TIMER1_BASE  0x80   // Base address for Timer/Counter1 registers
#define TIMER2_BASE  0xB0   // Base address for Timer/Counter2 registers

#define ADC_BASE     0x78   // Base address for ADC registers

#define USART0_BASE  0xC0   // Base address for USART0 registers

#define SPI_BASE     0x4C   // Base address for SPI registers

#define TWI_BASE     0xB8   // Base address for TWI (I2C) registers

// GPIO pointers (cast base addresses to GPIO_TypeDef pointers)
#define GPIOB   (*(volatile GPIO_TypeDef*)(GPIOB_BASE))   // Pointer to GPIO Port B registers
#define GPIOC   (*(volatile GPIO_TypeDef*)(GPIOC_BASE))   // Pointer to GPIO Port C registers
#define GPIOD   (*(volatile GPIO_TypeDef*)(GPIOD_BASE))   // Pointer to GPIO Port D registers

#define GPIO_INPUT          0
#define GPIO_OUTPUT         1

#define GPIO_PULL_UP        1
#define GPIO_PULL_DOWN      0

// Timer pointers (cast base addresses to timer struct pointers)
#define TIMER0  (*(volatile TIMER8_TypeDef*)(TIMER0_BASE))    // Pointer to Timer/Counter0 registers (8-bit)
#define TIMER1  (*(volatile TIMER16_TypeDef*)(TIMER1_BASE))   // Pointer to Timer/Counter1 registers (16-bit)
#define TIMER2  (*(volatile TIMER8_TypeDef*)(TIMER2_BASE))    // Pointer to Timer/Counter2 registers (8-bit)

// ADC pointer (cast base address to ADC_TypeDef pointer)
#define ADC1    (*(volatile ADC_TypeDef*)(ADC_BASE))          // Pointer to ADC registers

// USART pointer (cast base address to USART_TypeDef pointer)
#define USART0  (*(volatile USART_TypeDef*)(USART0_BASE))     // Pointer to USART0 registers

// SPI pointer (cast base address to SPI_TypeDef pointer)
#define SPI1    (*(volatile SPI_TypeDef*)(SPI_BASE))          // Pointer to SPI registers


// SPI configuration macros for ATmega328P
#define SPI_MODE_MASTER      1
#define SPI_MODE_SLAVE       0

#define SPI_DATA_ORDER_MSB   0
#define SPI_DATA_ORDER_LSB   1

#define SPI_CLOCK_POL_LOW    0   // SCK idle low
#define SPI_CLOCK_POL_HIGH   1   // SCK idle high

#define SPI_CLOCK_PHASE_LEAD 0   // Sample on leading edge
#define SPI_CLOCK_PHASE_TRAIL 1  // Sample on trailing edge

// SPI Clock Rate Select (SPR1:SPR0 in SPCR, SPI2X in SPSR)
#define SPI_CLOCK_DIV4       0   // Fosc/4
#define SPI_CLOCK_DIV16      1   // Fosc/16
#define SPI_CLOCK_DIV64      2   // Fosc/64
#define SPI_CLOCK_DIV128     3   // Fosc/128
#define SPI_CLOCK_DOUBLE     1   // Double speed (SPI2X=1)

#define SPI_INT_DISABLE      0
#define SPI_INT_ENABLE       1

#define SPI_SS_PIN           2   // PB2
#define SPI_MOSI_PIN         3   // PB3
#define SPI_MISO_PIN         4   // PB4
#define SPI_SCK_PIN          5   // PB5

// SPI Control Register (SPCR) bit positions
#define SPI_SPE   6  // SPI Enable
#define SPI_DORD  5  // Data Order
#define SPI_MSTR  4  // Master/Slave Select
#define SPI_CPOL  3  // Clock Polarity
#define SPI_CPHA  2  // Clock Phase
#define SPI_SPR1  1  // SPI Clock Rate Select 1
#define SPI_SPR0  0  // SPI Clock Rate Select 0
#define SPI_SPIE  7  // SPI Interrupt Enable

// SPI SSPI_tatus Register (SPSR) bit positions
#define SPI_SPIF  7  // SPI Interrupt Flag
#define SPI_WCOL  6  // Write Collision Flag
#define SPI_SPI2X 0  // Double SPI Speed Bit

// TWI pointer (cast base address to TWI_TypeDef pointer)
#define TWI1    (*(volatile TWI_TypeDef*)(TWI_BASE))          // Pointer to TWI (I2C) registers

// Bit positions for TWI registers (ATmega328P)
#define TWI_ENABLE_ACK  (1 << 6)   // TWEA
#define TWI_START       (1 << 5)   // TWSTA
#define TWI_STOP        (1 << 4)   // TWSTO
#define TWI_ENABLE      (1 << 2)   // TWEN
#define TWI_INTERRUPT   (1 << 7)   // TWINT

#define TW_STATUS_MASK  0xF8
#define TW_START_VAL    0x08
#define TW_MT_SLA_ACK   0x18
#define TW_MT_DATA_ACK  0x28
#define TW_MR_SLA_ACK   0x40
#define TW_MR_DATA_ACK  0x50
#define TW_MR_DATA_NACK 0x58

// USART Control and Status Register A (UCSRA) bit positions

#define USART_U2X   (1 << 1)   // Double the USART Transmission Speed
#define USART_RXC   (1 << 7)   // USART Receive Complete
#define USART_TXC   (1 << 6)   // USART Transmit Complete
#define USART_UDRE  (1 << 5)   // USART Data Register Empty
#define USART_FE    (1 << 4)   // Frame Error
#define USART_DOR   (1 << 3)   // Data OverRun
#define USART_UPE   (1 << 2)   // USART Parity Error

// USART Control and Status Register B (UCSRB) bit positions
#define USART_RXCIE 7   // RX Complete Interrupt Enable
#define USART_TXCIE 6   // TX Complete Interrupt Enable
#define USART_UDRIE 5   // USART Data Register Empty Interrupt Enable
#define USART_RXEN  4   // Receiver Enable
#define USART_TXEN  3   // Transmitter Enable
#define USART_UCSZ2 2   // Character Size bit 2
#define USART_RXB8  1   // Receive Data Bit 8
#define USART_TXB8  0   // Transmit Data Bit 8

// USART Control and Status Register C (UCSRC) bit positions
#define USART_UMSEL1 7  // USART Mode Select bit 1
#define USART_UMSEL0 6  // USART Mode Select bit 0
#define USART_UPM1   5  // Parity Mode bit 1
#define USART_UPM0   4  // Parity Mode bit 0
#define USART_USBS   3  // Stop Bit Select
#define USART_UCSZ1  2  // Character Size bit 1
#define USART_UCSZ0  1  // Character Size bit 0
#define USART_UCPOL  0  // Clock Polarity


#endif // ATMEGA328P_H   // End of include guard