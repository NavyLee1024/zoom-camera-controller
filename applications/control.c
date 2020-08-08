/**
 * @desc: �������߳�(�����Ź���ʼ��)
 */

#include "control.h"
#include "stepper.h"
#include "uart.h"

#define LOG_TAG             "control"
#include <drv_log.h>

#define WDT_DEVICE_NAME    "wdt"    /* ���Ź��豸���� */

static rt_device_t wdg_dev;    /* ���Ź��豸��� */

/* �߳���� */
static void control_thread_entry(void* parameter)
{
  
	while (1)
	{
		stepper_control(&focusStepper);
		stepper_control(&zoomStepper);
		
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
		rt_thread_startup(tid);
		
	/* ����������ų�ʼ�� */
	stepper_init(&zoomStepper);
	stepper_init(&focusStepper);
	LOG_I("stepper init");
  return 0;
}
INIT_APP_EXPORT(control_init);


/************************************************ ���Ź��߳� **********************************************/

static void wdg_thread_entry(void *parameter)
{
	while(1)
	{
		rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_KEEPALIVE, NULL);
		rt_thread_mdelay(100); // 100ms ι��һ��
	}
}

static int wdg_init(void)
{
	rt_thread_t tid = RT_NULL;
	rt_err_t ret = RT_EOK;
	rt_uint32_t timeout = 1;  /* ���ʱ�䣬��λ���� */
	
	/* �����豸���Ʋ��ҿ��Ź��豸����ȡ�豸��� */
	wdg_dev = rt_device_find(WDT_DEVICE_NAME);
	if (!wdg_dev)
	{
		LOG_E("find %s failed!\n", WDT_DEVICE_NAME);
		return RT_ERROR;
	}
	/* ��ʼ���豸 */
	ret = rt_device_init(wdg_dev);
	if (ret != RT_EOK)
	{
		rt_kprintf("initialize %s failed!\n", WDT_DEVICE_NAME);
		return RT_ERROR;
	}
	/* ���ÿ��Ź����ʱ�� */
	ret = rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_SET_TIMEOUT, &timeout);
	if (ret != RT_EOK)
	{
		rt_kprintf("set %s timeout failed!\n", WDT_DEVICE_NAME);
		return RT_ERROR;
	}
	/* �������Ź� */
	ret = rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_START, RT_NULL);
	if (ret != RT_EOK)
	{
		rt_kprintf("start %s failed!\n", WDT_DEVICE_NAME);
		return -RT_ERROR;
	}
	/* �����߳� ����ι��*/
	tid = rt_thread_create("control",
													wdg_thread_entry, 
													RT_NULL,
													1024,
													30, 10);
  if (tid != RT_NULL)
	{			
		rt_thread_startup(tid);
		LOG_I("%s init", wdg_dev);
	}
	
  return ret;
}
INIT_APP_EXPORT(wdg_init);

