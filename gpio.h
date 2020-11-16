/**
 * @file gpio.h
 * @author Daniel M. David L. Oliver W. 
 * @brief  Definitionen und Strukturen sowie Adressen der GPIOs 
 * @version 0.1
 * @date 2020-06-01
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#if !defined(GPIO_H)
#define GPIO_H

#define HIGH 1
#define LOW 0

typedef void (*interrupt_ptr_t)(void *);

#include "iocon.h"
typedef enum {
  INPUT = 0,
  INPUT_PULLUP,
  INPUT_PULLUP_HYS,
  INPUT_PULLDOWN,
  OUTPUT
} GPIO_mode_t;


typedef enum {
  FALLING = 0,
  RISING,
  BOTH,
  NONE
} GPIO_intr_mode_t;

//Register overview: GPIO (base address 0x2009 8000)

#define LPC_GPIO_BASE            0x20098000
#define LPC_GPIO                ((LPC_GPIO_T            *) LPC_GPIO_BASE)
#define LPC_GPIO_INTR_BASE       0x40028080
#define LPC_GPIO_INTR           ((LPC_GPIO_INTR_T       *) LPC_GPIO_INTR_BASE)

#define ISER1_REGISTER          ((uint32_t*) 0xE000E104) // Interrupt Set-Enable Register 1
#define ISE_GPIO_MASK 0x40 // Bit 6
#define ICPR0_REGISTER (uint32_t*)0xE000E280  ////Interrupt Clear-Pending Register 0


/**
 * @brief Struktur die die interrupt register im speicher für einen port bescheibt 
 * 
 */
typedef struct GPIOPORTINT_
{
    volatile const uint32_t stat_r;
    volatile const uint32_t stat_f;
    volatile uint32_t clr;
    volatile uint32_t enable_r;
    volatile uint32_t enable_f;
} LPC_GPIO_PORT_INTR;

/**
 * @brief Struktur die GPIO interrupt register bescheibt
 * 
 */
typedef struct GPIOINT_
{
    volatile const uint32_t status;
    volatile LPC_GPIO_PORT_INTR port0;
    const volatile uint32_t _reserved[3];
    volatile LPC_GPIO_PORT_INTR port2;
} LPC_GPIO_INTR_T;


/**
 * @brief Struktur die alle register im speicher für einen Port mit 32 GPIO pins im LPC beschreibt 
 */
typedef struct GPIOPORT_
{
    volatile uint32_t dir;     // pin direction register
	volatile const uint32_t _reserverd[3];
    volatile uint32_t mask;    // pin mask register
    volatile uint32_t pin;     // pin input register
    volatile uint32_t set;     // pin set register
    volatile uint32_t clr;     // pin clear register
} LPC_GPIO_PORT;


/**
 * @brief Struktur die alle 5 GPIO ports im speicher des LPC's bescheibt
 * 
 */
typedef struct GPIO_
{
    volatile LPC_GPIO_PORT p[5];
} LPC_GPIO_T;


/**
 * @brief Bit-Field for setting type D IOCON configuration
 * 
 */
typedef struct IOCON_D_GPIO_
{
    volatile uint32_t func  : 3;
    volatile uint32_t mode  : 2;
    volatile uint32_t hys   : 1;
    volatile uint32_t inv   : 1;
    volatile uint32_t slew  : 1;
    volatile uint32_t od    : 1;
} IOCON_D_GPIO_T;


/**
 * @brief Struct indem alle pointer zu den config-registern zwischengespeichert werden sowie bitmaske für den pin
 */
typedef struct GPIOPIN_
{
    volatile IOCON_D_GPIO_T* iocon;
    volatile LPC_GPIO_PORT* port;
    volatile uint32_t bitmask;
    volatile LPC_GPIO_PORT_INTR* intr_port;
} GPIO_pin;


void gpio_init_interrupts(void);
GPIO_pin gpio_init(uint8_t port, uint8_t pin);
void gpio_set_mode(GPIO_pin * pin, GPIO_mode_t mode);
uint8_t gpio_read(GPIO_pin* pin);
void gpio_write(GPIO_pin* pin, uint8_t value);


void gpio_interrupt(GPIO_pin * pin, GPIO_intr_mode_t mode, interrupt_ptr_t func_ptr, void * args);



#endif // GPIO_H
