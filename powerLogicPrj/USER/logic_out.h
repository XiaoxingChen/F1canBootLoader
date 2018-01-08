#ifndef LOGIC_OUT_H
#define LOGIC_OUT_H

#define BATT_EN_PIN				GPIO_Pin_6
#define BATT_EN_GPIO			GPIOA

#define BRD_nEN_PIN				GPIO_Pin_4
#define BRD_nEN_GPIO			GPIOA

#define PC_EN_PIN					GPIO_Pin_8
#define PC_EN_GPIO				GPIOA

#define launch_board()		GPIO_ResetBits(BRD_nEN_GPIO, BRD_nEN_PIN)
#define shutdown_board()	GPIO_SetBits(BRD_nEN_GPIO, BRD_nEN_PIN)

#define enable_battery()	GPIO_SetBits(BATT_EN_GPIO, BATT_EN_PIN)
#define disable_battery()	GPIO_ResetBits(BATT_EN_GPIO, BATT_EN_PIN)

#define pc_en_line_low()  GPIO_ResetBits(PC_EN_GPIO, PC_EN_PIN)
#define pc_en_line_high() GPIO_SetBits(PC_EN_GPIO, PC_EN_PIN)

void logic_pin_config(void);

#endif
//end of file
