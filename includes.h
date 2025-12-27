#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>  // needed for 'atof'

#include "stm32f7xx.h"
#include "stm32f769xx.h"
#include "stm32f7xx_hal_gpio.h" // needed for GPIO PULLUP stuff 

#include "version.h"

#include "pll.h"
#include "ports.h"
#include "ticker.h"
#include "uart6.h"
#include "i2c.h"
#include "fmc.h"
#include "dsi.h"
#include "otm8009a.h"
#include "ft6336.h"
#include "printf.h"
#include "dump.h"
