/**
 * @desc: A4988Ϊ��������������Ƴ���
 */

#include "pwm.h"

#define LOG_TAG             "pwm"
#include <drv_log.h>

#include <stdlib.h>
/* defined the Stepper Dir pin: B3 B6 */
#define FOCUS_DIR_PIN    GET_PIN(B, 3)
#define ZOOM_DIR_PIN     GET_PIN(B, 6)

#define PWM_DEV_NAME        "pwm3"  /* PWM�豸���� */
#define PWM_DEV_CHANNEL1     1       /* PWMͨ�� */
#define PWM_DEV_CHANNEL2     2       /* PWMͨ�� */
static struct rt_device_pwm *pwm_dev;      /* PWM�豸��� */

/* �������� ö��  0�� 1��*/
typedef enum 
{
		ANTICLOCKWISE, // ��ʱ�� 
		CLOCKWISE,     // ˳ʱ��
}stepperDir_e;

/* ���������ر� ö�� 0�� 1��*/
typedef enum 
{
		STEPPER_OFF, // �ر�
		STEPPER_ON , // ����
}stepperOnOff_e;

/* ������� �ṹ��*/
typedef struct 
{	
		char *name;					 /* ����������� */
		char *pwm_dev_name;  /* PWM�豸���� */
	 	rt_uint8_t    channel; /* PWMͨ�� */
		rt_uint32_t   period;  /* ���ڣ���λΪ����ns */
		rt_uint32_t   pulse;   /* ռ�ձȣ���λΪ����ns */
		rt_uint32_t   dir_pin; /* �������� */

}stepper_t;

/* ��ʼ���� PWM�豸��ͨ�������ŵ� */
stepper_t focusStepper = { // PWM CH1/B3
		.name         = "focus",
		.pwm_dev_name = PWM_DEV_NAME,
		.channel      = PWM_DEV_CHANNEL1,
		.period       = 2000000,   /* ����Ϊ2ms����λΪ����ns */
		.pulse        = 1000000,   /* ռ�ձ�1ms����λΪ����ns */
		.dir_pin      = FOCUS_DIR_PIN,

};

stepper_t zoomStepper = { // PWM CH2/B6
		.name         = "zoom",
		.pwm_dev_name = PWM_DEV_NAME,
		.channel      = PWM_DEV_CHANNEL2,
		.period       = 2000000,   /* ����Ϊ2ms����λΪ����ns */
		.pulse        = 1000000,   /* ռ�ձ�1ms����λΪ����ns */
		.dir_pin      = ZOOM_DIR_PIN,
};



void stepper_control(stepper_t *stepper,stepperOnOff_e onoff,stepperDir_e dir)
{
		if(ANTICLOCKWISE == dir) 
				rt_pin_write(stepper->dir_pin, PIN_HIGH); // ��ʱ��0
		else if(CLOCKWISE == dir)
				rt_pin_write(stepper->dir_pin, PIN_LOW);	// ˳ʱ��1
		else // stepperDir param error"
				return ;
				


	
		if(STEPPER_ON == onoff)
				rt_pwm_set(pwm_dev, stepper->channel, stepper->period, stepper->pulse); // ��������
		else if(STEPPER_OFF == onoff)
		{
				rt_pwm_set(pwm_dev, stepper->channel, stepper->period, 0); // �ر�����
				return; // �ر���ֱ�ӷ���
		}
		else // stepperOnOff param error
				return;
		

		
}
stepperOnOff_e msh_onoff;
stepperDir_e   msh_dir;

static int stepper_set(int argc, char *argv[])
{
		if(argc != 3)
		{
				LOG_E("Params error! Usage: stepper_set <on-off> <dir> \n\
							 eg. stepper_set 1 1 (on-clockwise)");
				return -RT_ERROR;
		}
		msh_onoff = (stepperOnOff_e)atoi(argv[1]);
		msh_dir   = (stepperDir_e)  atoi(argv[2]);
		return 0;
		
}
MSH_CMD_EXPORT(stepper_set, eg: stepper_set 1 1);


/* �߳���� */
static void thread_entry(void* parameter)
{
    /* �����豸 */
    pwm_dev = (struct rt_device_pwm *)rt_device_find(PWM_DEV_NAME);
    if (RT_NULL == pwm_dev)
    {
        rt_kprintf("pwm sample run failed! can't find %s device!\n", PWM_DEV_NAME);
        return;
    }
		else
				LOG_I("pwm3 init success");

    /* ʹ���豸 */
    rt_pwm_enable(pwm_dev, PWM_DEV_CHANNEL1);
		rt_pwm_enable(pwm_dev, PWM_DEV_CHANNEL2);

    while (1)
    {
				stepper_control(&focusStepper,msh_onoff, msh_dir);
				stepper_control(&zoomStepper ,msh_onoff, msh_dir);
				rt_thread_mdelay(10);
    }
}

static int pwm_init(void)
{
    rt_thread_t tid = RT_NULL;
    /* �����߳� 1 */
    tid = rt_thread_create("stepper",
                            thread_entry, RT_NULL,
                            1024,
                            10, 10);
    if (tid != RT_NULL)
        rt_thread_startup(tid);

    rt_pin_mode(focusStepper.dir_pin, PIN_MODE_OUTPUT);
	  rt_pin_mode(zoomStepper.dir_pin , PIN_MODE_OUTPUT);
	
		rt_pin_write(focusStepper.dir_pin, PIN_HIGH);
		rt_pin_write(zoomStepper.dir_pin , PIN_HIGH);
    return 0;
}
INIT_APP_EXPORT(pwm_init);
