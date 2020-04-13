/*
 * bsp_config.h
 *
 *  Created on: Jun 25, 2017
 *      Author: julian
 */

#ifndef LIBBSP_NUCLEO_F103_BSP_CONFIG_H_
#define LIBBSP_NUCLEO_F103_BSP_CONFIG_H_

/**
 * BSP global enabled definitions.
 */
#define BSP_ENABLED                     1

/**
 * BSP global disabled definitions.
 */
#define BSP_DISABLED                    0

/**
 * If enabled the bsp implements the sys tick interrupt and runs a tick counter.
 */
#define BSP_SYSTICK                     BSP_ENABLED

/**
 * Baud rate of the serial interface
 */
#define BSP_TTY_BAUDRATE                115200

/**
 * If enabled printf will use DMA
 */
#define BSP_TTY_TX_DMA                  BSP_ENABLED

/**
 * Defines the fifo size used for TTY DMA transmissions.
 */
#define BSP_TTY_TX_BUFSIZ               160

/**
 * If enabled the RX interrupt will be enabled and the incoming data will be
 * written to a internal ring buffer.
 */
#define BSP_TTY_RX_IRQ                  BSP_ENABLED

/**
 * Defines the RX fifo size used in case of enabled RX interrupt.
 */
#define BSP_TTY_RX_BUFSIZ               16

/**
 * If enabled _write (and therefore printf) will block until everything has
 * been written to the fifo. If not data which does not fit to the fifo will
 * be discarded.
 */
#define BSP_TTY_BLOCKING                BSP_ENABLED

/**
 * Interrupt priority configuration.
 *
 * See bsp.h for min max vaules and how they should be interpreted.
 */
#define BSP_IRQPRIO_SYSTICK               BSP_IRQPRIO_MAX
#define BSP_IRQPRIO_EXTI                  (BSP_IRQPRIO_MAX + 1)
#define BSP_IRQPRIO_TTY                   (BSP_IRQPRIO_MAX + 2)
/**
 * GPIO definitions.
 *
 * The bsp will define all GPIO's within a enumeration type called bspGpioPin_t
 * in the following way BSP_GPIO_<PORT><PIN>. Here those names can be replaced
 * by redefining them to define special function pins.
 *
 * ATTENTION:  Hence that there are such definitions within bsp.h for those pins
 *             the BSP uses internally like the serial port etc.
 */

/**
 * Debug pins for use with a logic analyzer.
 */
#define BSP_GPIO_C14                    BSP_DEBUGPIN_0
#define BSP_GPIO_C15                    BSP_DEBUGPIN_1
#define BSP_GPIO_C2                     BSP_DEBUGPIN_2
#define BSP_GPIO_C3                     BSP_DEBUGPIN_3

/**
 * If enabled bsp_assert.h will implement assertions.
 */
#define BSP_ASSERT                        BSP_ENABLED

/**
 * Defines the period of the LED flashes in case of a asseetion
 */
#define BSP_ASSERT_LED_MS                 100

/**
 * If enabled assertions will cause a message on stderr
 */
#define BSP_ASSERT_MESSAGE                BSP_ENABLED

/**
 * Defines the intervall between asseet messages in ms
 */
#define BSP_ASSERT_MESSAGE_MS             5000
#endif /* LIBBSP_NUCLEO_F103_BSP_CONFIG_H_ */
