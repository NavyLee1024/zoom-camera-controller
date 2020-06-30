/**
 * @desc: ��������������Ƴ���
 */

#include "stepper.h"

#define LOG_TAG             "stepper"
#include <drv_log.h>
#include <stdlib.h>


static rt_uint8_t forward_table[4] = {0x09,0x0A,0x06,0x05};  //4����ת�� Forward
static rt_uint8_t reverse_table[4] = {0x05,0x06,0x0A,0x09};  //4�ķ�ת�� Reverse



/* ZOOM���������ʼ���� ���ơ����ŵ� */
stepper_t zoomStepper = { // ��Զ����
	.name   = "zoom",

	.Ap_pin = GET_PIN(B, 3), /* A+ */
	.Am_pin = GET_PIN(B, 4), /* A- */
	.Bp_pin = GET_PIN(B, 5), /* B+ */
	.Bm_pin = GET_PIN(B, 6), /* B- */
};

/* FOCUS���������ʼ���� ���ơ����ŵ� */
stepper_t focusStepper = { // �Ŵ���С
	.name   = "focus",

	.Ap_pin = GET_PIN(B, 7),  /* A+ */
	.Am_pin = GET_PIN(B, 8),  /* A- */
	.Bp_pin = GET_PIN(B, 9),  /* B+ */
	.Bm_pin = GET_PIN(B, 10), /* B- */
};

/**
 * @brief  �������ʧֹͣʧ��
 * @param  *stepper ��������ṹ��ָ��
 */
void stepper_stop(stepper_t *stepper)
{
	rt_pin_write(stepper->Ap_pin, PIN_LOW);
	rt_pin_write(stepper->Am_pin, PIN_LOW);
	rt_pin_write(stepper->Bp_pin, PIN_LOW);
	rt_pin_write(stepper->Bm_pin, PIN_LOW);
}

/**
 * @brief  ����������ų�ʼ��
 * @param  *stepper ��������ṹ��ָ��
 */
void stepper_init(stepper_t *stepper)
{
	rt_pin_mode(stepper->Ap_pin, PIN_MODE_OUTPUT);
	rt_pin_mode(stepper->Am_pin, PIN_MODE_OUTPUT);
	rt_pin_mode(stepper->Bp_pin, PIN_MODE_OUTPUT);
	rt_pin_mode(stepper->Bm_pin, PIN_MODE_OUTPUT);

	/* ���ų�ʼ��������ֹͣ��ȷ���ȶ� */
	stepper_stop(stepper);
}

/**
 * @brief  ���ݽ��Ŀ�����ѡ���������
 * @param  *stepper ��������ṹ��ָ�룬beat ���Ŀ�����
 */
void selete_pin(rt_uint32_t pin, rt_uint8_t beat)
{
	if(beat)
		rt_pin_write(pin, PIN_HIGH);
	else
		rt_pin_write(pin, PIN_LOW);
}

/**
 * @brief  �����������
 * @param  *stepper ��������ṹ��ָ�룬*beat_table ���Ŀ��Ʊ��׵�ַ��*angle�Ƕȱ���ָ��
 */
void stepper_set(stepper_t *stepper, rt_uint8_t *beat_table, rt_int16_t *angle)
{

	/* �Ƕ��Լ����ȴ���ɶ�Ӧ�ĽǶ��� */
	while(((*angle)--) > 0) // ���������ֲ�δ��������ǣ��ݶ�Ϊ1����1��
	{
		for(int k = 0; k < 4; k++) // 4���ģ����һ������
		{
			selete_pin(stepper->Ap_pin,beat_table[k] & 0x08); /* A+ */
			selete_pin(stepper->Am_pin,beat_table[k] & 0x04); /* A- */
			selete_pin(stepper->Bp_pin,beat_table[k] & 0x02); /* B+ */
			selete_pin(stepper->Bm_pin,beat_table[k] & 0x01); /* B- */
			rt_thread_mdelay(1); // �ܲ�����ԼΪ1000pps�����������ʱ1ms
		}
	}
	stepper_stop(stepper); // �������ͣת
}

/**
 * @brief  �����������
 * @param  *stepper ��������ṹ��ָ�룬*dir ����ת��־ָ�룬*angle�Ƕȱ���ָ��(0~360)
 */
void stepper_control(stepper_t *stepper, stepper_dir_e *dir, rt_int16_t *angle)
{
	
	if(ANTICLOCKWISE == *dir)   // ��ת
		stepper_set(stepper,forward_table,angle);
	
	else if(CLOCKWISE == *dir)  // ��ת
		stepper_set(stepper,reverse_table,angle);
	
	*dir = STOP;
}

/**
 * @brief  ������� msh��������
 * @param  stepper_set <dir> <angle>
 */
static int stepper(int argc, char *argv[])
{
	int result = 0;
	stepper_dir_e stepper_dir;
	rt_int16_t    stepper_angle[2];

	if(argc != 3)
	{
		LOG_E("params error! Usage: stepper_set <dir> <angle>\r\n eg. stepper_set 1 120");
		result = -RT_ERROR;
		goto exit;
	}

	stepper_dir      = (stepper_dir_e)atoi(argv[1]); // ��ȡ �����־
	stepper_angle[0] = atoi(argv[2]);    // ��ȡ �Ƕ���ֵ
	stepper_angle[1] = stepper_angle[0]; // �Ƕ����ڻ��Լ�������������������Ҫ��������
	
	if(stepper_angle[0] < 0 || stepper_angle[0] > 360)
	{
		LOG_E("angle range in (0,360)");
		result = -RT_ERROR;
		goto exit;
	}
	
	stepper_control(&zoomStepper , &stepper_dir,&stepper_angle[0]);
	stepper_control(&focusStepper, &stepper_dir,&stepper_angle[1]);
	
exit:
	return result;

}
MSH_CMD_EXPORT(stepper, stepper_set <dir> <angle>);


