/**
 * @desc: �������߳�
 */

#include "control.h"
#include "stepper.h"
#include "uart.h"

#define LOG_TAG             "control"
#include <drv_log.h>



/* �߳���� */
static void control_thread_entry(void* parameter)
{
    
    while (1)
    {
			stepper_control(&focusStepper, &cmd_data.zoom_dir, &cmd_data.zoom_angle);
			stepper_control(&zoomStepper, &cmd_data.focus_dir, &cmd_data.focus_angle);
			
			rt_thread_mdelay(10); // �˴������̣߳��Է����������߳�
    }
}

static int control_init(void)
{
	rt_thread_t tid = RT_NULL;
	/* �����߳� */
	tid = rt_thread_create("control",
													control_thread_entry, 
													RT_NULL,
													1024,
													10, 10);
  if (tid != RT_NULL)
	{			
		rt_thread_startup(tid);
		
		/* ����������ų�ʼ�� */
		stepper_init(&zoomStepper);
		stepper_init(&focusStepper);
		LOG_I("stepper init");
	}
  return 0;
}
INIT_APP_EXPORT(control_init);
