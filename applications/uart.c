/**
 * @desc: UART2�������ݽ�������NnaoPi�ı佹��������
 */

#include "uart.h"
#include "control.h"
#include <rtdevice.h>

#define LOG_TAG             "uart"
#include <drv_log.h>

#define CONTROL_UART_NAME   "uart2"

/* �������� �ṹ�� */
cmd_t cmd_data;

/* �ϲ���������͵ı佹�����������������ݰ��������򷵻�"ok"
 * Ҳ�ɱ�ʾ������ ����Ƿ���ڱ佹���������յ����������ݰ�������"ok"
*/
rt_int8_t send_buff[2] = "ok";


/* ���ڽ�����Ϣ���ź��� */
static struct rt_semaphore rx_sem;
rt_device_t camera_uart_device;

/* �������ݻص����� */
static rt_err_t uart_input(rt_device_t dev, rt_size_t size)
{
    /* ���ڽ��յ����ݺ�����жϣ����ô˻ص�������Ȼ���ͽ����ź��� */
    rt_sem_release(&rx_sem);

    return RT_EOK;
}

static void serial_thread_entry(void *parameter)
{
    unsigned char  ch; 
    while (1)
    {
        /* �Ӵ��ڶ�ȡһ���ֽڵ����ݣ�û�ж�ȡ����ȴ������ź��� */
        while (rt_device_read(camera_uart_device, -1, &ch, 1) != 1)
        {
            /* �����ȴ������ź������ȵ��ź������ٴζ�ȡ���� */
            rt_sem_take(&rx_sem, RT_WAITING_FOREVER);
        }
				control_data_analysis(ch, &cmd_data);
    }
}

static int uart2_init(void)
{
    /* ����ϵͳ�еĴ����豸 */
    camera_uart_device = rt_device_find(CONTROL_UART_NAME);
    if (!camera_uart_device)
    {
        LOG_E("find %s failed!\n", CONTROL_UART_NAME);
        return RT_ERROR;
    }
		else
				LOG_I("%s init success", camera_uart_device);


		/* ��ʼ���ź��� */
    rt_sem_init(&rx_sem, "rx_sem", 0, RT_IPC_FLAG_FIFO);
    /* ���жϽ��ռ���ѯ����ģʽ�򿪴����豸 */
    rt_device_open(camera_uart_device, RT_DEVICE_FLAG_INT_RX);
    /* ���ý��ջص����� */
    rt_device_set_rx_indicate(camera_uart_device, uart_input);
		
		/* �������͸����ݰ�����ʾ�佹������� */
    rt_device_write(camera_uart_device, 0, send_buff, (sizeof(send_buff)));
    /* ���� serial �߳� */
    rt_thread_t thread = rt_thread_create("uart", serial_thread_entry, RT_NULL, 1024, 10, 10);
    /* �����ɹ��������߳� */
    if (thread != RT_NULL)
    {
        rt_thread_startup(thread);
    }

		return 0;
}
INIT_APP_EXPORT(uart2_init);

/**
 * @brief  �����������
 * @param  data �����֣� *cmd��������ṹ��ָ��
 */
void control_data_analysis(rt_uint8_t data, cmd_t *cmd) //�������ݽ���
{
	static rt_uint8_t i;
	static rt_uint8_t rxBuffer[10] = {0}; // ���ݰ�
	static rt_uint8_t rxCheck = 0;        // βУ����
	static rt_uint8_t rxCount = 0;        // ���ռ���
	
	rxBuffer[rxCount++] = data; // ���յ������ݴ��뻺������
	if(rxCount > 9)
		rxCount = 0;

	if (rxBuffer[0] != 0xAA && rxBuffer[1] != 0x55) // ����ͷ
	{
		// ����ͷ���ԣ������¿�ʼѰ��0xAA��0X55����ͷ
		rxCount = 0; // ��ջ�����
		return;
	}
	if (rxCount < CONTROL_PACKET_LENGTH)
		return; // ���ݲ���6�����򷵻�

	/*********** ֻ�н�����11���ֽ����� �Ż�������³��� ************/
	
	for (i = 0; i < CONTROL_PACKET_LENGTH - 1; i++)
		rxCheck += rxBuffer[i]; //У��λ�ۼ�	
	
	if (rxCheck == rxBuffer[CONTROL_PACKET_LENGTH - 1]) // �ж����ݰ�У���Ƿ���ȷ
	{

		cmd->zoom_dir  = (stepper_dir_e)rxBuffer[3];
		cmd->focus_dir = (stepper_dir_e)rxBuffer[4];
		
		cmd->focus_angle = STEP_ANGLE; // ÿ��ת��STEP_ANGLE��
		cmd->zoom_angle  = STEP_ANGLE;
		/* ���ݰ�����������"ok" */
		rt_device_write(camera_uart_device, 0, send_buff, (sizeof(send_buff)));
	}	

	rxCount = 0; // ��ջ�����
	rxCheck = 0; // У��λ����

}

