#ifndef COLOUR_H
#define COLOUR_H

#include "ui_widget.h"
//#include "widget.h"
#include "tools.h"
#include "HidCom.h"

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QSlider>

#define DATA_RGB_LIST_MAX	9//列表上限

#define DATA_RGB                (light_data)
#define DATA_RGB_T_D            (DATA_RGB + KEY_NUM * 3 * 2)
#define DATA_RGB_T_U            (DATA_RGB_T_D + KEY_NUM)
#define DATA_RGB_SHLD           (DATA_RGB_T_U + KEY_NUM)
#define DATA_RGB_IDX            (DATA_RGB_SHLD + KEY_NUM)
#define DATA_RGB_LIST           (DATA_RGB + 224)
#define DATA_RGB_COUNT          (DATA_RGB + 235)
#define DATA_RGB_MONO           (DATA_RGB + 238)
#define DATA_RGB_DIR            (DATA_RGB + 240)
#define DATA_RGB_WAVE           (DATA_RGB + 241)
#define DATA_RGB_COLOR_S        (DATA_RGB + 242)
#define DATA_RGB_COLOR_T        (DATA_RGB + 243)
#define DATA_RGB_T_WAIT         ((uint16_t*)(DATA_RGB + 244))
#define DATA_RGB_T_ACT          ((uint16_t*)(DATA_RGB + 246))
#define DATA_RGB_T_GAP          ((uint16_t*)(DATA_RGB + 248))
#define DATA_RGB_T_RAND         ((uint16_t*)(DATA_RGB + 250))
#define DATA_RGB_G_ON           (DATA_RGB + 252)
#define DATA_RGB_G_OFF          (DATA_RGB + 253)
#define DATA_RGB_M_ON           (DATA_RGB + 254)
#define DATA_RGB_M_OFF          (DATA_RGB + 255)

class Widget;

class colour : public QWidget
{
    Q_OBJECT
public:
    explicit colour(Widget *parent = nullptr);
    void openLightFile();
    void saveLightFile(uint8_t ifSave);
    void lightCfgToStr(uint8_t key_i);//配置数据转标签字符串
    
    Widget *widget = NULL;
    
    QLabel *lb_rgbs[KEY_NUM * 2], *lb_rgbs_set[KEY_NUM];
    QPushButton* bt_rgbs[KEY_NUM];
    //QComboBox* cbbox_rgbs[KEY_NUM];
    QSpinBox* sbox_rgbs[KEY_NUM * 3];
    
    QSpinBox* sbox_plt[6];
    QSlider* slider_plt[6];
    QLabel* lb_plt[6];
    
    QComboBox* list[DATA_RGB_LIST_MAX];
    
    uint8_t *light_data = NULL;//灯效数组
    
    QString fileNow = "";//现在打开的文件
    
signals:
    
public slots:
    void on_keys_clicked();
    void on_rgbs_valueChanged(int arg1);
    void on_color_upDown_changed(int arg1);
    void on_color_plt_changed(int arg1);
    void on_open_clicked();
    void on_save_clicked();
    void on_saveas_clicked();
    void on_write_clicked();
    void on_import_clicked();
    void on_export_clicked();
};

#endif // COLOUR_H
