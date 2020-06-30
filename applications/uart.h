#ifndef __MY_UART_H__
#define __MY_UART_H__

#include <rtthread.h>
#include <board.h>
#include "stepper.h"

/* �������ݰ����� 6 */
#define CONTROL_PACKET_LENGTH  6

/* �佹�������� */
typedef struct
{
	stepper_dir_e zoom_dir;
	stepper_dir_e focus_dir;
	
	rt_int16_t zoom_angle;
	rt_int16_t focus_angle;
	
}cmd_t;

/* �������ݽ��� */
void control_data_analysis(rt_uint8_t data, cmd_t *cmd);

/* �������� �ṹ�� */
extern cmd_t cmd_data;

#endif
