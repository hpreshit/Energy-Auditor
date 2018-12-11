//***********************************************************************************
// Include files
//***********************************************************************************
//#include "main.h"
#include "em_gpio.h"

//***********************************************************************************
// defined files
//***********************************************************************************

// LED0 pin is
#ifdef GATEWAY
#define	LED0_port		gpioPortB
#define LED0_pin		11
#define LED0_default	false 	// off
// LED1 pin is
#define LED1_port		gpioPortB
#define LED1_pin		13
#define LED1_default	false	// off

#else

#define	LED0_port		gpioPortF
#define LED0_pin		4
#define LED0_default	false 	// off
// LED1 pin is
#define LED1_port		gpioPortF
#define LED1_pin		5
#define LED1_default	false	// off

#endif
//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************
void gpio_init(void);

