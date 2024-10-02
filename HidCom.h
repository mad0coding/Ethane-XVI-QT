#ifndef HIDCOM_H
#define HIDCOM_H

#include <QWidget>


#define CHID_OK             0
#define CHID_NO_DEV         1
#define CHID_MULTI_DEV      2
#define CHID_ERR_INIT       3
#define CHID_ERR_OPEN       4
#define CHID_ERR_CLOSE      5
#define CHID_ERR_WRITE      6
#define CHID_ERR_READ       7
#define CHID_BAD_REP        8


#define CHID_CMD_CFG_KEY    0
#define CHID_CMD_CFG_LIGHT  1
#define CHID_CMD_KEY_FLT    2
#define CHID_CMD_RK_ADC     3
#define CHID_CMD_EC_FREQ    4
#define CHID_CMD_RST        10
#define CHID_CMD_BOOT       11
#define CHID_CMD_FLASH_CNT  12


QString CHID_to_str(uint8_t ret);//HID返回值转字符串

uint8_t hid_init(void);//HID初始化
uint8_t hid_set_para(uint16_t fdVid, uint16_t fdPid, uint16_t fdUsagePage);//HID查找参数设置

uint8_t hid_send_cmd(uint8_t cmd, uint8_t *inBuf, uint8_t *outBuf);//HID向设备写入命令
uint8_t hid_send_data(uint8_t cmd, uint8_t *inBuf, uint8_t *buf);//HID向设备写入数据













#endif // HIDCOM_H
