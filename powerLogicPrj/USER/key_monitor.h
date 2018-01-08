#ifndef KEY_MONITOR_H
#define KEY_MONITOR_H
#include <stdint.h>
typedef enum
{
	RELEASED,
	PRESSED,
	RELEASING,
	PRESSING,
	NO_SUCH_KEY
}key_state;

enum KEY_ENUM{
	KEY_IS_PC_LAUNCH = 0,
	KEY_LAUNCH,
	KEY_BRAKE,
	KEY_DCIN,
	KEY_HY_ON,
	KEY_HY_OFF,
	KEY_ON_BRD,
	TOTAL_KEY_NUM
};

typedef struct key_struct key_type;
void key_monitor_run();
void key_init();
key_state key_get_data(int key_index);
key_state key_get_raw_data(int key_index);
uint8_t key_is_long_pressed(int key_index);


#endif
//end of file
