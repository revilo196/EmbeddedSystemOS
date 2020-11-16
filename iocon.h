/**
 * @file iocon.h
 * @author O.Walter D.Melzer
 * @brief  Definition der IOCON configration
 * @version 0.1
 * @date 2020-05-26
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#if !defined(IOCON_H)
#define IOCON_H

#include <stdint.h>

#define LPC_IOCON_BASE            0x4002C000
#define LPC_IOCON                 ((LPC_IOCON_T            *) LPC_IOCON_BASE)

#define IOCON_FUNC0             0x000				/*!< Selects pin function 0 */
#define IOCON_FUNC1             0x001				/*!< Selects pin function 1 */
#define IOCON_FUNC2             0x002				/*!< Selects pin function 2 */
#define IOCON_FUNC3             0x003				/*!< Selects pin function 3 */

/**
 * @brief IOCON register block
 */
typedef struct {
	volatile uint32_t p[5][32];
} LPC_IOCON_T;




#endif // IOCON_H
