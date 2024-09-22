#ifndef WIDGET_H
#define WIDGET_H

#include "colour.h"
#include "config.h"
#include "tools.h"

#include <QWidget>
#include "windows.h"
#include <iostream>

#include <QInputDialog>
#include <QColor>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT
    
public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    
    HidApi *hidApi = NULL;//HID
    HidDevice *hidDev = NULL;//HID设备
    
    config *cfgUnit;//配置类
    colour *clrUnit;//色彩类
    
    QString fileNow = "";//现在打开的文件
    QString fileNowKey = "";//现在打开的key文件
    
    volatile bool ifSending = false;//是否正在发送
    volatile bool response = false;//设备是否响应
    volatile uint8_t count = 255;//设备响应计数
    
    volatile bool ifHold = false;//功能键是否按键保持
    volatile uint8_t func = 0;//功能键按下情况
    
    volatile uint8_t state = 0;//状态
    volatile uint8_t button_class = 0;//按钮分类
    volatile uint8_t button_choose = 0;//按钮选择
    
    volatile uint8_t key_num = 0;//按下按键数量
    
    volatile bool ifPos = false;//是否处于位置获取
    
    QString style_big_black, style_big_gray;//样式设定
    QString style_mid_black, style_mid_gray;//样式设定
    QString style_small_black, style_small_gray;//样式设定
    
public:
    bool hid_write_read(uint8_t *writeBuf, uint8_t *readBuf);//HID先写后读
    uint32_t connectHID(uint8_t cmd);//以自定义HID连接设备
    bool writeHID(uint8_t mode, uint8_t *buf);//以自定义HID设备写入数据
    
    void openCfgFile();//打开配置文件
    void saveCfgFile();//保存配置文件
    void openKeyFile();//打开key文件
    void saveKeyFile();//保存key文件
    
private slots:
    void key_handle(uint8_t keyValue, bool ifPress);//按键处理
    void keyPressEvent(QKeyEvent *event);//按键按下
    void keyReleaseEvent(QKeyEvent *event);//按键抬起
    
    void passPointer();//传递指针

    void key_click_handle(uint8_t button_cs);//键盘页16按钮按下处理
    void on_key1_clicked();
    void on_key2_clicked();
    void on_key3_clicked();
    void on_key4_clicked();
    void on_key5_clicked();
    void on_key6_clicked();
    void on_key7_clicked();
    void on_key8_clicked();
    void on_key9_clicked();
    void on_key10_clicked();
    void on_key11_clicked();
    void on_key12_clicked();
    void on_key13_clicked();
    void on_key14_clicked();
    void on_key15_clicked();
    void on_key16_clicked();
    
    void key_r_click_handle(uint8_t button_cs);//摇杆页10按钮按下处理
    void on_key_r_1_clicked();
    void on_key_r_1_0_clicked();
    void on_key_r_1_1_clicked();
    void on_key_r_1_2_clicked();
    void on_key_r_1_3_clicked();
//    void on_key_r_2_clicked();
//    void on_key_r_2_0_clicked();
//    void on_key_r_2_1_clicked();
//    void on_key_r_2_2_clicked();
//    void on_key_r_2_3_clicked();
    
    void key_e_click_handle(uint8_t button_cs);//旋钮页6按钮按下处理
    void on_key_e_1_clicked();
    void on_key_e_1_0_clicked();
    void on_key_e_1_1_clicked();
    void on_key_e_2_clicked();
    void on_key_e_2_0_clicked();
    void on_key_e_2_1_clicked();
    
    void on_Bt_hold_clicked();//按键保持
    
    void on_Bt_write_clicked();//写入设备按钮
    void on_Bt_open_clicked();//打开配置文件按钮
    void on_Bt_save_clicked();//保存配置文件按钮
    void on_Bt_saveas_clicked();//另存为配置文件按钮
    
    void on_Bt_write_key_clicked();//写入key按钮
    void on_Bt_open_key_clicked();//打开key文件按钮
    void on_Bt_save_key_clicked();//保存key文件按钮
    void on_Bt_saveas_key_clicked();//另存为key文件按钮
    
    void on_mode3Box_cursorPositionChanged();//文本框光标位置改变
    void on_mode3Box_textChanged();//文本框文本改变
    void on_mode3_input_clicked();//按键组输入按钮
    void on_mode3_shortcut_clicked();//按键组快捷键按钮
    void on_mode3_del_clicked();//按键组删除按钮
    void on_mode3_clear_clicked();//按键组清空按钮
    
    void on_key_clear_clicked();//按键清除键
    void on_key_clear_r_1_clicked();//摇杆按键清除键
    //void on_key_clear_r_2_clicked();//摇杆按键清除键
    void on_key_clear_e_1_clicked();//旋钮按键清除键
    void on_key_clear_e_2_clicked();//旋钮按键清除键
    
    void on_tabWidget_currentChanged(int index);//翻页
    void State_to_zero();//状态归零
    
    void on_Bt_mouse_L_clicked();//鼠标左键
    void on_Bt_mouse_M_clicked();//鼠标中键
    void on_Bt_mouse_R_clicked();//鼠标右键
    
    void on_Bt_wheel_up_clicked();//鼠标滚轮向上
    void on_Bt_wheel_down_clicked();//鼠标滚轮向下
    
    void on_Bt_vol_stop_clicked();//媒体停止
    void on_Bt_vol_mute_clicked();//媒体静音
    void on_Bt_vol_up_clicked();//媒体音量加
    void on_Bt_vol_down_clicked();//媒体音量减
    void on_Bt_vol_next_clicked();//媒体下一个
    void on_Bt_vol_prev_clicked();//媒体上一个
    
    void on_Bt_empty_clicked();//空按键
    
    void on_Bt_pos_get_clicked();//位置捕获
    void on_Bt_pos_input_clicked();//位置填入
    
    void on_Bt_delay_clicked();//时间填入
    
    void on_mode3_loop_clicked();//循环
    
    void on_Bt_glb_key_flt_clicked();//按键滤波
    void on_Bt_glb_rk_calib_clicked();//摇杆校正
    void on_Bt_glb_ec_freq_clicked();//旋钮设置
    
    void on_spinBox_rgb_r_valueChanged(int arg1);
    void on_spinBox_rgb_g_valueChanged(int arg1);
    void on_spinBox_rgb_b_valueChanged(int arg1);
    void sys_rgb_display();//显示rgb值
    
public://private:
    Ui::Widget *ui;
};

#endif // WIDGET_H










