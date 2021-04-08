//
// Created by Ethan Wesley on 16/11/2020.
//

#ifndef PINS_H
#define PINS_H

#define PIN_SPI_SS_ETHERNET_LIB 70

#define ETHERNET_CHIP_SELECT 70
#define ETHERNET_INTERRUPT 71
#define RTC_CHIP_SELECT 72
#define RTC_INTERRUPT 73

#define OVEN_STEP_PIN       5   //CONTROLLINO_D3
#define OVEN_DIR_PIN        8   //CONTROLLINO_D6
#define OVEN_DISABLE_PIN    7   //CONTROLLINO_D5

#define GLASS_STEP_PIN      6   //CONTROLLINO_D4
#define GLASS_DIR_PIN       3   //CONTROLLINO_D1
#define GLASS_DISABLE_PIN   2   //CONTROLLINO_D0

#define SOL1_PIN            11   //CONTROLLINO_D9
#define SOL2_PIN            12   //CONTROLLINO_D10
#define SOL3_PIN            13   //CONTROLLINO_D11

#define VAC_RELAY_PIN       9    //CONTROLLINO_D7
#define OVEN_RELAY_PIN      10   //CONTROLLINO_D8

#define LIMIT_BOT           A1   //CONTROLLINO_A0
#define LIMIT_TOP           A0   //CONTROLLINO_A1

#define GLASS_TEMP_PIN      A7   //CONTROLLINO_A6
#define MANDREL_TEMP_PIN    A6   //CONTROLLINO_A7
#define VAC_PRES_PIN        A8   //CONTROLLINO_A8

#define BTNUP               A2   //CONTROLLINO_A2
#define BTNDOWN             A3   //CONTROLLINO_A3
#define BTNGREEN            18   //CONTROLLINO_INT0
#define BTNRED              19   //CONTROLLINO_INT1
#define LTGREEN             24   //CONTROLLINO_R2
#define LTRED               25   //CONTROLLINO_R2

#endif //PINS_H
