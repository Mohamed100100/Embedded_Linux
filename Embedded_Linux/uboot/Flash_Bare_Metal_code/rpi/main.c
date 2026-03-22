/*
 * main.c - Bare-metal LED blinky for Raspberry Pi 3B+
 * 
 * Controls GPIO pin to blink an LED
 * Uses BCM2837 peripheral registers at physical addresses
 */

#include <stdint.h>

/* ============================================================
 * Configuration (can be overridden by CMake)
 * ============================================================ */

#ifndef LED_GPIO_PIN
#define LED_GPIO_PIN 26
#endif

#ifndef BLINK_DELAY
#define BLINK_DELAY 500000
#endif

/* ============================================================
 * BCM2837 GPIO Register Definitions
 * The Raspberry Pi 3B+ uses BCM2837 SoC
 * GPIO peripheral base is at 0x3F200000
 * ============================================================ */

#define BCM2837_GPIO_BASE   0x3F200000UL

/* GPIO Function Select Registers */
#define GPFSEL0     ((volatile uint32_t *)(BCM2837_GPIO_BASE + 0x00))
#define GPFSEL1     ((volatile uint32_t *)(BCM2837_GPIO_BASE + 0x04))
#define GPFSEL2     ((volatile uint32_t *)(BCM2837_GPIO_BASE + 0x08))

/* GPIO Pin Output Set Registers */
#define GPSET0      ((volatile uint32_t *)(BCM2837_GPIO_BASE + 0x1C))

/* GPIO Pin Output Clear Registers */
#define GPCLR0      ((volatile uint32_t *)(BCM2837_GPIO_BASE + 0x28))

/* GPIO Pull-up/down Registers */
#define GPPUD       ((volatile uint32_t *)(BCM2837_GPIO_BASE + 0x94))
#define GPPUDCLK0   ((volatile uint32_t *)(BCM2837_GPIO_BASE + 0x98))

/* Function select values */
#define GPIO_FUNC_INPUT     0x0
#define GPIO_FUNC_OUTPUT    0x1

/* ============================================================
 * Delay Function
 * Simple busy-wait delay using CPU cycles
 * ============================================================ */

static void delay(uint32_t count)
{
    volatile uint32_t i;
    for (i = 0; i < count; i++) {
        __asm__ volatile("nop");
    }
}

/* ============================================================
 * GPIO Functions
 * ============================================================ */

/*
 * Set GPIO pin function (input, output, or alternate function)
 */
static void gpio_set_function(uint32_t pin, uint32_t function)
{
    uint32_t reg_index = pin / 10;
    uint32_t bit_offset = (pin % 10) * 3;
    volatile uint32_t *gpfsel = (volatile uint32_t *)(uintptr_t)(BCM2837_GPIO_BASE + (reg_index * 4));
    
    uint32_t value = *gpfsel;
    value &= ~(0x7U << bit_offset);
    value |= (function << bit_offset);
    *gpfsel = value;
}

/*
 * Set GPIO pin HIGH
 */
static void gpio_set(uint32_t pin)
{
    *GPSET0 = (1U << pin);
}

/*
 * Set GPIO pin LOW
 */
static void gpio_clear(uint32_t pin)
{
    *GPCLR0 = (1U << pin);
}

/*
 * Initialize GPIO pin for LED
 */
static void led_init(void)
{
    gpio_set_function(LED_GPIO_PIN, GPIO_FUNC_OUTPUT);
    gpio_clear(LED_GPIO_PIN);
}

/*
 * Turn LED on
 */
static void led_on(void)
{
    gpio_set(LED_GPIO_PIN);
}

/*
 * Turn LED off  
 */
static void led_off(void)
{
    gpio_clear(LED_GPIO_PIN);
}

/* ============================================================
 * Main Function
 * ============================================================ */

void main(void)
{
    led_init();
    
    while (1) {
        led_on();
        delay(BLINK_DELAY);
        
        led_off();
        delay(BLINK_DELAY);
    }
    
    /* Should never reach here */
    while (1) {
        __asm__ volatile("wfe");
    }
}