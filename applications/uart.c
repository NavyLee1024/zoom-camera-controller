/**
 * @desc: UART2�������ݽ�������NnaoPi�ı佹��������
 */

#include "uart.h"
#include <rtdevice.h>

#define LOG_TAG             "uart"
#include <drv_log.h>

#define CONTROL_UART_NAME   "uart2"

/* ���ڽ�����Ϣ���ź��� */
static struct rt_semaphore rx_sem;
static rt_device_t serial;

/* �������ݻص����� */
static rt_err_t uart_input(rt_device_t dev, rt_size_t size)
{
    /* ���ڽ��յ����ݺ�����жϣ����ô˻ص�������Ȼ���ͽ����ź��� */
    rt_sem_release(&rx_sem);

    return RT_EOK;
}

static void serial_thread_entry(void *parameter)
{
    char ch;

    while (1)
    {
        /* �Ӵ��ڶ�ȡһ���ֽڵ����ݣ�û�ж�ȡ����ȴ������ź��� */
        while (rt_device_read(serial, -1, &ch, 1) != 1)
        {
            /* �����ȴ������ź������ȵ��ź������ٴζ�ȡ���� */
            rt_sem_take(&rx_sem, RT_WAITING_FOREVER);
        }
        /* ��ȡ��������ͨ�����ڴ�λ��� */
        ch = ch + 1;
        rt_device_write(serial, 0, &ch, 1);
    }
}

static int uart2_init(void)
{
		rt_thread_t tid;
	
    /* ����ϵͳ�еĴ����豸 */
    serial = rt_device_find(CONTROL_UART_NAME);
    if (!serial)
    {
        LOG_E("find %s failed!\n", CONTROL_UART_NAME);
        return RT_ERROR;
    }
		else
		{
				LOG_I("%s init success", serial);
		}

    /* ��ʼ���ź��� */
    rt_sem_init(&rx_sem, "rx_sem", 0, RT_IPC_FLAG_FIFO);
    /* ���жϽ��ռ���ѯ����ģʽ�򿪴����豸 */
    rt_device_open(serial, RT_DEVICE_FLAG_INT_RX);
    /* ���ý��ջص����� */
    rt_device_set_rx_indicate(serial, uart_input);

    /* ���� serial �߳� */
		tid = rt_thread_create("uart",
												serial_thread_entry, RT_NULL,
												1024,
												8, 10);

    if (tid != RT_NULL)      
				rt_thread_startup(tid);

		return 0;
}
INIT_APP_EXPORT(uart2_init);
