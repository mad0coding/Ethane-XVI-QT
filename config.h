#ifndef CONFIG_H
#define CONFIG_H

//#include "widget.h"
#include "tools.h"
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QPlainTextEdit>
#include <QVector>

struct config_key;

class config
{
public:
    config();
    
    void set_mode1_key(uint8_t key_i, uint8_t key_val);//设置模式1按键
    void set_mode2_key(uint8_t key_i, uint8_t key_val, uint8_t func);//设置模式2按键
    bool add_mode3_key(uint8_t key_i, uint8_t key_val, uint8_t shift);//增加模式3按键
    bool add_mode3_func(uint8_t key_i, uint8_t key_val, uint8_t func);//增加模式3快捷键
    bool add_mode3_pos(uint8_t key_i, uint16_t x, uint16_t y);//增加模式3光标移位
    bool add_mode3_delay(uint8_t key_i, uint16_t t);//增加模式3延时
    bool add_mode3_loop(uint8_t key_i, uint8_t func, uint16_t n);//增加模式3循环
    bool add_mode3_report(uint8_t key_i, uint8_t func);//增加模式3报文控制
    void set_mode4_key(uint8_t key_i, uint16_t x, uint16_t y);//设置模式4(5)按键
    void set_mode6_key(uint8_t key_i, uint8_t key_val, uint8_t func);//设置模式6按键及切换
    void set_mode7_key(uint8_t key_i, uint8_t key_val, uint8_t func);//设置模式7按键及配置
    void set_mode7_time(uint8_t key_i, uint16_t t);//设置模式7时间
    void set_rk_key(uint8_t key_i, uint8_t key_val);//设置摇杆按键
    void set_ec_key(uint8_t key_i, uint8_t key_val);//设置旋钮按键
    void set_mode3_cursor(uint8_t key_i, uint16_t pos);//设置模式3光标
    void set_mode3_txt(uint8_t key_i);//设置模式3显示文本
    //void set_mode3_str(QString str);//直接设置模式3显示文本
    void add_mode3_unit(uint8_t key_i, QVector<uint8_t> vct, QString str);//增加模式3单元
    void del_mode3_key(uint8_t key_i);//删除一个模式3按键
    void clear_key(uint8_t key_i);//清除模式1~3按键
    void clear_rk_key(uint8_t key_i);//清除摇杆按键
    void clear_ec_key(uint8_t key_i);//清除旋钮按键
    
    bool write_cfg_data();//写入配置数组
    bool read_cfg_data();//读出配置数组
    
    bool write_mode3_data(uint8_t keyi);//写入key配置数组
    bool read_mode3_data(uint8_t keyi);//读出key配置数组
    
    void set_key_mode(uint8_t keyi, uint8_t mode);//设置按键模式
    uint8_t get_key_mode(uint8_t keyi);//获取按键模式
    
    
    uint8_t *cfg_data = NULL;//配置数组
    uint8_t *cfg_data_key = NULL;//key配置数组
    
    config_key *cfg_key = NULL;//按键配置
    
    uint8_t *rk_key = NULL;//摇杆按键
    uint8_t *ec_key = NULL;//旋钮按键
    uint8_t *rk_key_func = NULL;//摇杆按键func
    uint8_t *ec_key_func = NULL;//旋钮按键func
    
    //控件指针:
    QLabel* label_k[KEY_NUM];//按键标签
    QPushButton* bt_k[KEY_NUM];//按键
    QComboBox* cbox_k[KEY_NUM];//按键模式
    QComboBox *cbox_key_turn;//键盘方向

    QComboBox *cbox_r[RK_NUM],*cbox_r_key[RK_NUM];//摇杆模式 按键模式
    QSpinBox *sbox_v[RK_NUM],*sbox_r[RK_NUM];//速度 死区
    QCheckBox *ckbox_r_x[RK_NUM],*ckbox_r_y[RK_NUM],*ckbox_r_r[RK_NUM];//3类反向
    QPushButton* bt_r[RK_NUM * 5];//按键
    QLabel* label_r[RK_NUM * 5];//按键标签
    
    QComboBox *cbox_e[EC_NUM],*cbox_e_key[EC_NUM];//旋钮模式 按键模式
    QCheckBox *ckbox_e[EC_NUM];//反向
    QPushButton* bt_e[EC_NUM * 3];//按键
    QLabel* label_e[EC_NUM * 3];//按键标签
    
    QComboBox *cbox_wave,*cbox_colorful;//RGB呼吸 RGB变色
    QComboBox *cbox_rgb_delay;//配置切换指示延时
    QComboBox *cbox_pri;//配置优先级
    
    //QSpinBox *sbox_light,*sbox_light_rgb;
    QSpinBox *sbox_w,*sbox_h;//屏幕宽度 屏幕高度
    QSpinBox *sbox_rgb_r,*sbox_rgb_g,*sbox_rgb_b;//RGB色值
    QSpinBox *sbox_rgb_light;//RGB亮度
    
    QCheckBox *ckbox_sys_rgb_rk,*ckbox_sys_rgb_clicker,*ckbox_sys_rgb_m3;//摇杆映射 自动连点指示 按键组指示
    
    QPlainTextEdit *pte_mode3Box;//文本框
    uint8_t m3boxTextFlag = 0;//修改文本标志位
};

struct config_key
{
    uint8_t mode = 0;//模式
    uint8_t key = 0;//键值
    uint8_t func = 0;//功能键
    QString str = "";//文本
    QVector<uint8_t> data = {};//数据列
    QVector<uint8_t> strNum = {};//文本分段
    QVector<uint8_t> dataNum = {};//数据列分段
    uint16_t cursor = 0;//光标位置
    
    uint16_t x = 0, y = 0;
    uint16_t t = 0;
};


#endif // CONFIG_H



