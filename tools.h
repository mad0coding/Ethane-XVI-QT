#ifndef TOOLS_H
#define TOOLS_H

//#include "widget.h"
#include <QCoreApplication>
#include <QTime>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QString>
#include <QDebug>
//#include <QSerialPort>
//#include <QSerialPortInfo>
#include <QWidget>
#include <QPixmap>
#include <QDesktopWidget>


#define m1_button    1
#define m2_shortcut  2
#define m3_group     3
#define m4_move      4
#define m5_click     5
#define m6_change    6
#define m7_fast      7
//#define m8_light     8
//#define m9_serial    9
#define m8_buzz     8

#define kv_report       234
#define kv_loop         235

#define kv_point		237
#define kv_vol_next     238
#define kv_vol_prev     239
#define kv_wheel_up     240
#define kv_wheel_down   241
#define kv_vol_up       242
#define kv_vol_down     243
#define kv_vol_mute     244
#define kv_vol_stop     245
#define kv_mouse_l      246
#define kv_mouse_m      247
#define kv_mouse_r      248
#define kv_ctrl         249
#define kv_shift        250
#define kv_alt          251
#define kv_win          252
#define kv_shortcut     253
#define kv_delay		254

#define LIMIT(x,min,max)	(((x)<=(min))?(min):(((x)>=(max))?(max):(x)))//上下限
#define MIN(X, Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))
#define colorAngle 600//色环的1/6

#define RK_NUM  1//摇杆个数
#define EC_NUM  2//旋钮个数
#define KEY_NUM  16//按键个数

bool read_asciiArray_file(QString path, uint8_t *buf, uint16_t len);//读明文数组文件
bool read_cfg_file(QString path, uint8_t *buf, uint16_t len);//读配置文件
bool write_cfg_file(QString path, uint8_t *buf, uint16_t len);//写配置文件

//bool read_key_file(QString path, uint8_t *buf);//读key文件
//bool write_key_file(QString path, uint8_t *buf);//写key文件

void mySleep(int ms);//毫秒延时

uint8_t key_to_USB(int key, int Vkey);//QT键值转USB键值
QString USB_to_str(uint8_t key, bool shift);//USB键值转按键名

void rgbToHsv(uint8_t vR, uint8_t vG, uint8_t vB, uint16_t* pH, uint16_t* pS, uint16_t* pV);
void hsvToRgb(uint16_t vH, uint16_t vS, uint16_t vV, uint8_t* pR, uint8_t* pG, uint8_t* pB);

QPixmap rgbToPix(uint8_t r, uint8_t g, uint8_t b, uint16_t w, uint16_t h, uint8_t mode);

uint16_t EndianConvert16(uint16_t num);//16位大小端转换


#endif // TOOLS_H




