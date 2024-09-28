#include "colour.h"

#include "widget.h"

colour::colour(Widget *parent) : QWidget(parent)
{
    widget = parent;
    
    light_data = new uint8_t[256];
    memset(light_data, 0, 256);
    
    connect(widget->ui->cBox_color_updown, 
            static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), 
            this, &colour::on_color_upDown_changed);
    connect(widget->ui->Bt_import_color, &QPushButton::clicked, this, &colour::on_import_clicked);
    connect(widget->ui->Bt_export_color, &QPushButton::clicked, this, &colour::on_export_clicked);
    connect(widget->ui->Bt_open_color, &QPushButton::clicked, this, &colour::on_open_clicked);
    connect(widget->ui->Bt_save_color, &QPushButton::clicked, this, &colour::on_save_clicked);
    connect(widget->ui->Bt_saveas_color, &QPushButton::clicked, this, &colour::on_saveas_clicked);
    connect(widget->ui->Bt_write_color, &QPushButton::clicked, this, &colour::on_write_clicked);
    
    for(int yi = 0; yi < 4; yi++){//行
        for(int xi = 0; xi < 4; xi++){//列
            int i = yi * 4 + xi;
            int x = xi * 140 + 10, y = yi * 140 + 10;
            
            bt_rgbs[i] = new QPushButton(widget->ui->tab_color);//按键
            bt_rgbs[i]->setGeometry(x + 50, y + 70, 60, 40);
            bt_rgbs[i]->setText("KEY" + QString::number(i + 1));
            connect(bt_rgbs[i], &QPushButton::clicked, this, &colour::on_keys_clicked);
            
//            cbbox_rgbs[i] = new QComboBox(widget->ui->tab_color);//下拉框
//            cbbox_rgbs[i]->setGeometry(x, y + 90, 100, 30);
//            cbbox_rgbs[i]->addItem("普通",0);
//            cbbox_rgbs[i]->addItem("十字",1);
//            cbbox_rgbs[i]->addItem("X形",2);
//            cbbox_rgbs[i]->addItem("方块",3);
            
            for(int j = 0; j < 3; j++){//RGB数值框
                sbox_rgbs[i*3 + j] = new QSpinBox(widget->ui->tab_color);
                sbox_rgbs[i*3 + j]->setGeometry(x, y + j*30 + 30, 50, 30);
                sbox_rgbs[i*3 + j]->setMaximum(255);
                
                connect(sbox_rgbs[i*3 + j], 
                        static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), 
                        this,&colour::on_rgbs_valueChanged);
            }
            
            lb_rgbs[i*2] = new QLabel(widget->ui->tab_color);
            lb_rgbs[i*2]->setGeometry(x + 50, y + 30, 30, 30);
            lb_rgbs[i*2]->setPixmap(rgbToPix(
                sbox_rgbs[i*3]->value(), sbox_rgbs[i*3 + 1]->value(), sbox_rgbs[i*3 + 2]->value(), 
                    30, 30, 0));//图片添加到标签控件
            lb_rgbs[i*2 + 1] = new QLabel(widget->ui->tab_color);
            lb_rgbs[i*2 + 1]->setGeometry(x + 50 + 30, y + 30, 30, 30);
            lb_rgbs[i*2 + 1]->setPixmap(rgbToPix(
                sbox_rgbs[i*3]->value(), sbox_rgbs[i*3 + 1]->value(), sbox_rgbs[i*3 + 2]->value(), 
                    30, 30, 1));//图片添加到标签控件
            lb_rgbs_set[i] = new QLabel(widget->ui->tab_color);
            lb_rgbs_set[i]->setGeometry(x, y, 120, 30);
            lb_rgbs_set[i]->setText("0 0 0 0");//文本添加到标签控件
        }
    }
    
    lb_plt[0] = widget->ui->lb_plt_r;
    lb_plt[1] = widget->ui->lb_plt_g;
    lb_plt[2] = widget->ui->lb_plt_b;
    lb_plt[3] = widget->ui->lb_plt_h;
    lb_plt[4] = widget->ui->lb_plt_s;
    lb_plt[5] = widget->ui->lb_plt_v;
    sbox_plt[0] = widget->ui->sBox_plt_r;
    sbox_plt[1] = widget->ui->sBox_plt_g;
    sbox_plt[2] = widget->ui->sBox_plt_b;
    sbox_plt[3] = widget->ui->sBox_plt_h;
    sbox_plt[4] = widget->ui->sBox_plt_s;
    sbox_plt[5] = widget->ui->sBox_plt_v;
    slider_plt[0] = widget->ui->hSlider_r;
    slider_plt[1] = widget->ui->hSlider_g;
    slider_plt[2] = widget->ui->hSlider_b;
    slider_plt[3] = widget->ui->hSlider_h;
    slider_plt[4] = widget->ui->hSlider_s;
    slider_plt[5] = widget->ui->hSlider_v;
    for(int i = 0; i < 6; i++){
        connect(sbox_plt[i], static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), 
                this, &colour::on_color_plt_changed);
        connect(slider_plt[i], static_cast<void (QSlider::*)(int)>(&QSlider::valueChanged), 
                this, &colour::on_color_plt_changed);
    }
    on_color_plt_changed(0);//直接调用调色盘变化槽函数完成初始化
}

void colour::on_keys_clicked(){//按钮点击
    QObject *object = QObject::sender();//获取发送者
    uint16_t i = 0;
    for(i = 0; i < KEY_NUM; i++){//检查是哪个按键
        if(object == bt_rgbs[i]) break;
    }
    if(i >= KEY_NUM) return;
    if(widget->ui->tabWidget_light->currentIndex() == 0){//若在色盘这一页
        for(int j = 0; j < 3; j++) sbox_rgbs[i*3 + j]->setValue(sbox_plt[j]->value());//色盘颜色填入按键
    }else if(widget->ui->tabWidget_light->currentIndex() == 1){//若在单键这一页
        uint8_t setInWhat = widget->ui->cBox_color_setin->currentIndex();//填入内容选择
        if(setInWhat == 0 || setInWhat == 3){//填入延迟
            DATA_RGB_T1[i] = widget->ui->sBox_color_t1->value();
            DATA_RGB_T2[i] = widget->ui->sBox_color_t2->value();
        }
        if(setInWhat == 1 || setInWhat == 3){//填入自定义次序
            DATA_RGB_IDX[i] = widget->ui->sBox_color_i1->value();
        }
        if(setInWhat == 2 || setInWhat == 3){//填入屏蔽剩余
            DATA_RGB_SHLD[i] = widget->ui->sBox_color_shield->value();
        }
        lightCfgToStr(i);//配置数据转标签字符串
    }
}

void colour::on_rgbs_valueChanged(int arg1){//rgb色值改变
    QObject *object = QObject::sender();//获取发送者
    uint16_t i = 0;
    for(i = 0; i < KEY_NUM*3; i++){
        if(object == sbox_rgbs[i]) break;
    }
    if(i >= KEY_NUM*3) return;
    int offset = widget->ui->cBox_color_updown->currentIndex() * KEY_NUM * 3;
    DATA_RGB[offset + i] = sbox_rgbs[i]->value();//存入数组
    i = i / 3;
    lb_rgbs[i*2]->setPixmap(rgbToPix(
        sbox_rgbs[i*3]->value(), sbox_rgbs[i*3 + 1]->value(), sbox_rgbs[i*3 + 2]->value(), 
            30, 30, 0));//图片添加到标签控件
    lb_rgbs[i*2 + 1]->setPixmap(rgbToPix(
        sbox_rgbs[i*3]->value(), sbox_rgbs[i*3 + 1]->value(), sbox_rgbs[i*3 + 2]->value(), 
            30, 30, 1));//图片添加到标签控件
}

void colour::on_color_upDown_changed(int arg1){//上下层颜色切换
    int offset = widget->ui->cBox_color_updown->currentIndex() * KEY_NUM * 3;
    for(int i = 0; i < KEY_NUM*3; i++){
        sbox_rgbs[i]->setValue(DATA_RGB[offset + i]);
    }
}

void colour::on_color_plt_changed(int arg1)//调色盘变化
{
    static uint8_t usingFlag = 0;//正在使用标志
    if(usingFlag) return;//防止无限嵌套
    usingFlag = 1;//使用标志置位
    
    QObject *object = QObject::sender();//获取发送者
    uint16_t rgbOrHsv = 0, ifSlider = 0;
    for(rgbOrHsv = 0; rgbOrHsv < 6; rgbOrHsv++){//匹配发送者
        if(object == sbox_plt[rgbOrHsv]){ifSlider = 0; break;}
        if(object == slider_plt[rgbOrHsv]){ifSlider = 1; break;}
    }
    uint16_t value[6];
    QColor color(0, 0, 0);
    if(rgbOrHsv < 3){//rgb
        if(ifSlider) value[0] = slider_plt[0]->value(), value[1] = slider_plt[1]->value(), value[2] = slider_plt[2]->value();
        else value[0] = sbox_plt[0]->value(), value[1] = sbox_plt[1]->value(), value[2] = sbox_plt[2]->value();
        color.setRgb(value[0], value[1], value[2]);
        value[3] = color.hue(); value[4] = color.saturation(); value[5] = color.value();
    }else{//hsv
        if(ifSlider) value[3] = slider_plt[3]->value(), value[4] = slider_plt[4]->value(), value[5] = slider_plt[5]->value();
        else value[3] = sbox_plt[3]->value(), value[4] = sbox_plt[4]->value(), value[5] = sbox_plt[5]->value();
        color.setHsv(value[3], value[4], value[5]);
        value[0] = color.red(); value[1] = color.green(); value[2] = color.blue();
    }
    
    widget->ui->lb_plt_pic_0->setPixmap(rgbToPix(value[0], value[1], value[2], 30, 30, 0));//图片添加到标签控件
    widget->ui->lb_plt_pic_1->setPixmap(rgbToPix(value[0], value[1], value[2], 30, 30, 1));//图片添加到标签控件
    
    const uint16_t rgb_hsv_max[6] = {255, 255, 255, 360, 255, 255};//各值的满幅
    QImage image(150, 10, QImage::Format_RGB888);
    for(int index = 0; index < 6; index++){//对rgb和hsv变化
        slider_plt[index]->setValue(value[index]);
        sbox_plt[index]->setValue(value[index]);
        uint16_t tmp = value[index];//暂存值
        for(int i = 0; i < 150; i++){//横向变化
            value[index] = i * rgb_hsv_max[index] / 150;
            if(index < 3) color.setRgb(value[0], value[1], value[2]);//rgb
            else color.setHsv(value[3], value[4], value[5]);//hsv
            QRgb pixel = qRgb(color.red(), color.green(), color.blue());
            for(int j = 0; j < 10; j++) image.setPixel(i, j, pixel);//纵向不变
        }
        value[index] = tmp;//恢复值
        lb_plt[index]->setPixmap(QPixmap::fromImage(image));//图片添加到标签控件
    }
    usingFlag = 0;//使用标志复位
}

void colour::openLightFile(){
    uint8_t u8data[256];
    memset(u8data, 0, 256);
    if(!read_cfg_file(fileNow, u8data, 256)){
        QMessageBox::critical(this,"提示","无路径");
        return;
    }
    
    memcpy(light_data, u8data, 256);//拷贝数据
    for(int i = 0; i < KEY_NUM; i++) lightCfgToStr(i);//配置数据转标签字符串
    widget->ui->cBox_color_updown->setCurrentIndex(!widget->ui->cBox_color_updown->currentIndex());//上下层翻转
    int offset = widget->ui->cBox_color_updown->currentIndex() * KEY_NUM * 3;
    for(int i = 0; i < KEY_NUM * 3; i++) sbox_rgbs[i]->setValue(u8data[offset + i]);
    widget->ui->cBox_color_updown->setCurrentIndex(!widget->ui->cBox_color_updown->currentIndex());//上下层翻转
    offset = widget->ui->cBox_color_updown->currentIndex() * KEY_NUM * 3;
    for(int i = 0; i < KEY_NUM * 3; i++) sbox_rgbs[i]->setValue(u8data[offset + i]);
    
    widget->ui->cBox_mono_light->setCurrentIndex(*DATA_RGB_MONO);
    widget->ui->cBox_dir_light->setCurrentIndex(*DATA_RGB_DIR);
    widget->ui->cBox_wave_light->setCurrentIndex(*DATA_RGB_WAVE);
    widget->ui->sBox_colorful_light->setValue(*DATA_RGB_COLORFUL);
    widget->ui->sBox_color_d1wave->setValue(EndianConvert16(*DATA_RGB_D1WAVE));
    widget->ui->sBox_color_d2wave->setValue(EndianConvert16(*DATA_RGB_D2WAVE));
    widget->ui->sBox_color_t1wave->setValue(*DATA_RGB_T1WAVE);
    widget->ui->sBox_color_t2wave->setValue(*DATA_RGB_T2WAVE);
    widget->ui->sBox_color_t1sys->setValue(*DATA_RGB_T1SYS);
    widget->ui->sBox_color_t2sys->setValue(*DATA_RGB_T2SYS);
    
    QFileInfo fileinfo = QFileInfo(fileNow);//文件信息
    widget->ui->lineEdit_fileName_light->setText(fileinfo.fileName());//显示文件名
}

void colour::saveLightFile(uint8_t ifSave){
    *DATA_RGB_MONO = widget->ui->cBox_mono_light->currentIndex();
    *DATA_RGB_DIR = widget->ui->cBox_dir_light->currentIndex();
    *DATA_RGB_WAVE = widget->ui->cBox_wave_light->currentIndex();
    *DATA_RGB_COLORFUL = widget->ui->sBox_colorful_light->value();
    *DATA_RGB_D1WAVE = EndianConvert16(widget->ui->sBox_color_d1wave->value());
    *DATA_RGB_D2WAVE = EndianConvert16(widget->ui->sBox_color_d2wave->value());
    *DATA_RGB_T1WAVE = widget->ui->sBox_color_t1wave->value();
    *DATA_RGB_T2WAVE = widget->ui->sBox_color_t2wave->value();
    *DATA_RGB_T1SYS = widget->ui->sBox_color_t1sys->value();
    *DATA_RGB_T2SYS = widget->ui->sBox_color_t2sys->value();
    
    if(!ifSave) return;//若仅填入数据不保存则退出
    
    QFileInfo fileinfo = QFileInfo(fileNow);//文件信息
    if(fileinfo.fileName().isEmpty()) return;
    if(fileinfo.suffix().isEmpty()){
        fileNow += ".etlight";//若无后缀则添加
        fileinfo = QFileInfo(fileNow);
    }
    write_cfg_file(fileNow,light_data,256);
    widget->ui->lineEdit_fileName_light->setText(fileinfo.fileName());//显示文件名
}

void colour::lightCfgToStr(uint8_t key_i){//配置数据转标签字符串
    QString str = "";
    str += QString::number(DATA_RGB_T1[key_i]) + " ";
    str += QString::number(DATA_RGB_T2[key_i]) + " ";
    
    str += QString::number(DATA_RGB_IDX[key_i]) + " ";
    
    str += QString::number(DATA_RGB_SHLD[key_i]);
//    if(DATA_RGB_SHLD[key_i]) str += "Y ";
//    else str += "N ";
//    if(DATA_RGB_IDX[key_i] == 0) str += "普通";
//    else if(DATA_RGB_IDX[key_i] == 1) str += "十字";
//    else if(DATA_RGB_IDX[key_i] == 2) str += "X形";
//    else if(DATA_RGB_IDX[key_i] == 3) str += "方形";
    lb_rgbs_set[key_i]->setText(str);//文本设置到标签
}

void colour::on_open_clicked(){//打开
    QString path = "./lightFile";
    QDir dir(path);
    if(!dir.exists()) path = "../lightFile";//若从文件打开exe则需要回退到根目录
    QString fileNew = QFileDialog::getOpenFileName(this, QStringLiteral("打开灯效文件"),
                                                   path, QStringLiteral("灯效文件(*etlight)"));
    if(fileNew.isEmpty()) return;
    fileNow = fileNew;
    openLightFile();//打开灯效文件
}
void colour::on_save_clicked(){//保存
    QString path = "./lightFile";
    QDir dir(path);
    if(!dir.exists()) path = "../lightFile";//若从文件打开exe则需要回退到根目录
    if(fileNow.isEmpty()){
        fileNow = QFileDialog::getSaveFileName(this, QStringLiteral("保存灯效文件"),
                                               path, QStringLiteral("灯效文件(*etlight)"));
    }
    saveLightFile(1);//保存灯效文件
}
void colour::on_saveas_clicked(){//另存为
    QString path = "./lightFile";
    QDir dir(path);
    if(!dir.exists()) path = "../lightFile";//若从文件打开exe则需要回退到根目录
    QString fileNew = QFileDialog::getSaveFileName(this, QStringLiteral("另存为灯效文件"),
                                                   path, QStringLiteral("灯效文件(*etlight)"));
    if(fileNew.isEmpty()) return;
    fileNow = fileNew;
    saveLightFile(1);//保存灯效文件
}
void colour::on_write_clicked(){//写入灯效按钮
    widget->ui->Bt_write_color->setStyleSheet(widget->style_mid_gray);
    widget->ui->Bt_write_color->setEnabled(false);//暂时禁用按钮
    
    saveLightFile(0);//仅填入控件的数据至数组而不保存至文件
    
    uint8_t cfgPos = widget->ui->cBox_flash_color->currentIndex();//填入灯效存储位置
    hid_set_para(widget->ui->spinBox_vid->value(), widget->ui->spinBox_pid->value(), 0xFF00);   //HID查找参数设置
    uint8_t ret = hid_send_data(CHID_CMD_CFG_LIGHT, &cfgPos, light_data);                       //HID发送数据
    
    if(ret == CHID_OK) ;//QMessageBox::information(this,"提示","写入成功");
    else QMessageBox::critical(this, "灯效写入", "HID通信失败\n" + CHID_to_str(ret));
    
    widget->ui->Bt_write_color->setEnabled(true);//恢复启用按钮
    widget->ui->Bt_write_color->setStyleSheet(widget->style_mid_black);
}
void colour::on_import_clicked(){//导入配色
    QString path = "./lightFile";
    QDir dir(path);
    if(!dir.exists()) path = "../lightFile";//若从文件打开exe则需要回退到根目录
    QString fileTmp = QFileDialog::getOpenFileName(this, QStringLiteral("导入配色文件"),
                                                   path, QStringLiteral("配色文件(*txt)"));
    if(fileTmp.isEmpty()) return;
    uint8_t u8color[KEY_NUM * 3];
    memset(u8color, 0, KEY_NUM * 3);
    if(!read_asciiArray_file(fileTmp, u8color, KEY_NUM * 3)){
        QMessageBox::critical(this,"提示","导入失败");
        return;
    }
    for(int i = 0; i < KEY_NUM * 3; i++) sbox_rgbs[i]->setValue(u8color[i]);
}
void colour::on_export_clicked(){//导出配色
    QString path = "./lightFile";
    QDir dir(path);
    if(!dir.exists()) path = "../lightFile";//若从文件打开exe则需要回退到根目录
    QString fileTmp = QFileDialog::getSaveFileName(this, QStringLiteral("导出配色文件"),
                                                   path, QStringLiteral("配色文件(*txt)"));
    QFileInfo fileinfo = QFileInfo(fileTmp);//文件信息
    if(fileinfo.fileName().isEmpty()) return;
    if(fileinfo.suffix().isEmpty()) fileTmp += ".txt";//若无后缀则添加
    
    int offset = widget->ui->cBox_color_updown->currentIndex() * KEY_NUM * 3;
    char charColor[KEY_NUM * 13];//每组RGB需要13字节
    for(int i = 0; i < KEY_NUM; i++){
        int j = i*3 + offset;
        sprintf(charColor + i*13,"%03d,%03d,%03d,",DATA_RGB[j],DATA_RGB[j+1],DATA_RGB[j+2]);
        if(i % 4 == 3) charColor[i*13 + 12] = '\n';
        else charColor[i*13 + 12] = ' ';
    }
    write_cfg_file(fileTmp,(uint8_t*)charColor,sizeof(charColor));
}












