#ifndef __STEPPER_H__
#define __STEPPER_H__

#include <rtthread.h>
#include <board.h>

/* ��ʼ��ʱ��ÿ�ν��ܵ�ָ��� �̶��Ĳ����� */
#define STEP_ANGLE  10

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
	rt_uint32_t   Ap_pin; /* A+ */
	rt_uint32_t   Am_pin; /* A- */
	rt_uint32_t   Bp_pin; /* B+ */
	rt_uint32_t   Bm_pin; /* B- */
	stepper_dir_e dir;    /* ���� */
	rt_int16_t    angle;  /* �Ƕ� */
	rt_int16_t    fix_angle; /* ÿ���յ�ָ��ת���Ĺ̶��Ƕ� */
}stepper_t;

extern stepper_t zoomStepper,focusStepper;

/* ����������ų�ʼ�� */
void stepper_init(stepper_t *stepper);
/* ����������� */
void stepper_control(stepper_t *stepper);
/* �趨����������� */
void set_stepper_params(stepper_t *stepper, uint8_t data);
#endif
