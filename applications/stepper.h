#ifndef __STEPPER_H__
#define __STEPPER_H__

#include <rtthread.h>
#include <board.h>

/* �������� ö��  0�� 1��*/
typedef enum 
{
	STOP          = 0, // ֹͣ
	ANTICLOCKWISE = 1, // ��ʱ�� ��ת 
	CLOCKWISE     = 2, // ˳ʱ�� ��ת
}stepper_dir_e;



/* ������� �ṹ��*/
typedef struct 
{	
	char *name;					 // ����������� 
	
	/* ����������Ŷ���*/
	rt_uint32_t  Ap_pin; /* A+ */
	rt_uint32_t  Am_pin; /* A- */
	rt_uint32_t  Bp_pin; /* B+ */
	rt_uint32_t  Bm_pin; /* B- */
	
}stepper_t;

extern stepper_t zoomStepper,focusStepper;

/* ����������ų�ʼ�� */
void stepper_init(stepper_t *stepper);
/* ����������� */
void stepper_control(stepper_t *stepper, stepper_dir_e *dir, rt_int16_t *angle);
#endif
