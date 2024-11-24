#include "widget.h"
#include "ui_widget.h"


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    
    clrUnit = new colour(this);//色彩类
    cfgUnit = new config;//配置类
    passPointer();//传递指针
    
    connect(ui->Bt_delay, 
            &QPushButton::clicked, this, 
            &Widget::sys_rgb_display);
    
    hid_init();//HID初始化
    
    sys_rgb_display();//显示rgb
    
    ui->label_ctrl->setStyleSheet("color: gray");
    ui->label_shift->setStyleSheet("color: gray");
    ui->label_alt->setStyleSheet("color: gray");
    ui->label_win->setStyleSheet("color: gray");
    
    QString style_prefix = "QPushButton{background-color:rgb(220, 220, 220);";
    QString style_suffix = "QPushButton:hover{background-color:rgb(210, 210, 210);}"
                           "QPushButton:pressed{background-color:rgb(200, 200, 200);}";
    
    style_big_black = style_prefix + "color: black;border-radius: 20px;}" + style_suffix;
    style_big_gray = style_prefix + "color: gray;border-radius: 20px;}" + style_suffix;
    style_mid_black = style_prefix + "color: black;border-radius: 12px;}" + style_suffix;
    style_mid_gray = style_prefix + "color: gray;border-radius: 12px;}" + style_suffix;
    style_small_black = style_prefix + "color: black;border-radius: 8px;}" + style_suffix;
    style_small_gray = style_prefix + "color: gray;border-radius: 8px;}" + style_suffix;
    
    for(int i = 0; i < KEY_NUM; i++){
        cfgUnit->cbox_k[i]->clear();
        //cfgUnit->cbox_k[i]->addItems(cbox_k_list);
        cfgUnit->cbox_k[i]->addItem("无",0);
        cfgUnit->cbox_k[i]->addItem("单键",1);
        cfgUnit->cbox_k[i]->addItem("快捷键",2);
        cfgUnit->cbox_k[i]->addItem("按键组",3);
        cfgUnit->cbox_k[i]->addItem("光标移位",4);
        cfgUnit->cbox_k[i]->addItem("光标点击",5);
        cfgUnit->cbox_k[i]->addItem("切换键",6);
        cfgUnit->cbox_k[i]->addItem("按键连点",7);
        cfgUnit->cbox_k[i]->addItem("蜂鸣器",8);
        
        cfgUnit->bt_k[i]->setStyleSheet(style_big_black);//键盘按键样式
        clrUnit->bt_rgbs[i]->setStyleSheet(style_small_black);//灯效键盘按键样式
    }
    for(int i = 0; i < RK_NUM*5; i++) cfgUnit->bt_r[i]->setStyleSheet(style_big_black);//摇杆按键样式
    for(int i = 0; i < EC_NUM*3; i++) cfgUnit->bt_e[i]->setStyleSheet(style_big_black);//旋钮按键样式
    
    ui->Bt_write->setStyleSheet(style_big_black);
    ui->Bt_open->setStyleSheet(style_big_black);
    ui->Bt_save->setStyleSheet(style_big_black);
    ui->Bt_saveas->setStyleSheet(style_big_black);
    ui->Bt_glb_rk_calib->setStyleSheet(style_big_black);
    ui->Bt_glb_ec_freq->setStyleSheet(style_big_black);
    ui->Bt_glb_key_flt->setStyleSheet(style_big_black);
    ui->Bt_special->setStyleSheet(style_big_black);
    ui->Bt_info->setStyleSheet(style_big_black);
    
    ui->key_clear->setStyleSheet(style_mid_black);
    ui->key_clear_e_1->setStyleSheet(style_mid_black);
    ui->key_clear_e_2->setStyleSheet(style_mid_black);
    ui->key_clear_r_1->setStyleSheet(style_mid_black);
    //ui->key_clear_r_2->setStyleSheet(style_mid_black);
    
    ui->Bt_pos_get->setStyleSheet(style_mid_black);
    ui->Bt_pos_input->setStyleSheet(style_mid_black);
    ui->Bt_delay->setStyleSheet(style_mid_black);
    
    ui->Bt_open_key->setStyleSheet(style_mid_black);
    ui->Bt_write_key->setStyleSheet(style_mid_black);
    ui->Bt_save_key->setStyleSheet(style_mid_black);
    ui->Bt_saveas_key->setStyleSheet(style_mid_black);
    ui->Bt_open_color->setStyleSheet(style_mid_black);
    ui->Bt_write_color->setStyleSheet(style_mid_black);
    ui->Bt_save_color->setStyleSheet(style_mid_black);
    ui->Bt_saveas_color->setStyleSheet(style_mid_black);
    ui->Bt_import_color->setStyleSheet(style_mid_black);
    ui->Bt_export_color->setStyleSheet(style_mid_black);
    
    ui->mode3_input->setStyleSheet(style_mid_black);
    ui->mode3_shortcut->setStyleSheet(style_mid_black);
    ui->mode3_loop->setStyleSheet(style_mid_black);
    ui->mode3_del->setStyleSheet(style_mid_black);
    ui->mode3_clear->setStyleSheet(style_mid_black);
    
    ui->Bt_hold->setStyleSheet(style_small_black);
    ui->Bt_empty->setStyleSheet(style_small_black);
    ui->Bt_vol_down->setStyleSheet(style_small_black);
    ui->Bt_vol_up->setStyleSheet(style_small_black);
    ui->Bt_vol_prev->setStyleSheet(style_small_black);
    ui->Bt_vol_next->setStyleSheet(style_small_black);
    ui->Bt_vol_mute->setStyleSheet(style_small_black);
    ui->Bt_vol_stop->setStyleSheet(style_small_black);
    ui->Bt_wheel_down->setStyleSheet(style_small_black);
    ui->Bt_wheel_up->setStyleSheet(style_small_black);
    ui->Bt_mouse_L->setStyleSheet(style_small_black);
    ui->Bt_mouse_M->setStyleSheet(style_small_black);
    ui->Bt_mouse_R->setStyleSheet(style_small_black);
    
//    QFont font = this->font();
//    printf("%d\n",font.pointSize());
//    font.setPointSize(7);//默认为9
//    this->setFont(font);
}

Widget::~Widget(){
    delete clrUnit;
    delete cfgUnit;
    delete ui;
}

void Widget::keyHandle(uint8_t keyValue, bool ifPress = true)//按键处理
{
    if(ifPress && state > 0){
        if(button_class == 1){//键盘
            if(cfgUnit->get_key_mode(button_choose - 1) == m1_button){//单键
                cfgUnit->set_mode1_key(button_choose - 1,keyValue);
                cfgUnit->bt_k[button_choose - 1]->setStyleSheet(style_big_black);
                state = 0;
            }
            else if(cfgUnit->get_key_mode(button_choose - 1) == m2_shortcut){//快捷键
                if(!(keyValue >= 249 && keyValue <= 252) && func != 0){//若不为功能键且功能键存在
                    cfgUnit->set_mode2_key(button_choose - 1,keyValue,func);
                    cfgUnit->bt_k[button_choose - 1]->setStyleSheet(style_big_black);
                    state = 0;
                }
            }
            else if(cfgUnit->get_key_mode(button_choose - 1) == m3_group){//按键组
                if(!(keyValue >= 249 && keyValue <= 252)){//若不为功能键
                    if(state == 2 && func != 0){
                        if(!cfgUnit->add_mode3_func(button_choose - 1,keyValue,func)){
                            QMessageBox::critical(this,"提示","此键可用空间不足");
                        }
                    }
                    else if(state == 3){
                        if(!cfgUnit->add_mode3_key(button_choose - 1,keyValue,func & 0x02)){
                            QMessageBox::critical(this,"提示","此键可用空间不足");
                        }
                    }
                }
            }
            else if(cfgUnit->get_key_mode(button_choose - 1) == m6_change){//配置切换
                int ansTP = QMessageBox::question(this, "切换配置", "切换方式?", "临时", "永久", 0, -1);
                bool ifOK = false;
                int ansTO = QInputDialog::getInt(this, "切换配置", "切换目标:1-8",
                                                 1, 1, 8, 1,//默认值,最小值,最大值,步进
                                                 &ifOK, Qt::WindowCloseButtonHint);
                if(ifOK){
                    if(ansTP == 0){//临时切换
                        cfgUnit->set_mode6_key(button_choose - 1, keyValue, ansTO | 0x80);
                    }
                    else{//永久切换
                        cfgUnit->set_mode6_key(button_choose - 1, 0, ansTO);
                    }
                }
                cfgUnit->bt_k[button_choose - 1]->setStyleSheet(style_big_black);
                state = 0;
            }
            else if(cfgUnit->get_key_mode(button_choose - 1) == m7_fast){//按键连点
                int ansAuto = QMessageBox::question(this,"按键连点","连点方式?","非自动","自动",0,-1);
                cfgUnit->set_mode7_key(button_choose - 1,keyValue,ansAuto);
                cfgUnit->bt_k[button_choose - 1]->setStyleSheet(style_big_black);
                state = 0;
            }
        }
        else if(button_class == 2){//摇杆
            if((button_choose - 1) % 5 == 0){//若为中心键
                if(!(keyValue >= 249 && keyValue <= 252)){//若不为功能键
                    cfgUnit->set_rk_key(button_choose - 1, keyValue, func);
                    cfgUnit->bt_r[button_choose - 1]->setStyleSheet(style_big_black);
                    state = 0;
                }
            }
            else{//若为四方向绑定键
                cfgUnit->set_rk_key(button_choose - 1, keyValue, 0);
                cfgUnit->bt_r[button_choose - 1]->setStyleSheet(style_big_black);
                state = 0;
            }
        }
        else if(button_class == 3){//旋钮
            if(!(keyValue >= 249 && keyValue <= 252)){//若不为功能键
                cfgUnit->set_ec_key(button_choose - 1, keyValue, func);
                cfgUnit->bt_e[button_choose - 1]->setStyleSheet(style_big_black);
                state = 0;
            }
        }
    }
    else if(ifPress && ui->tabWidget->currentIndex() == 4 && ui->tabWidget_light->currentIndex() == 1){//若在灯效页的按键页
        if(keyValue == 30 || keyValue == 22 || keyValue == 81) ui->sBox_color_i1->setValue(ui->sBox_color_i1->value() - 1);//1,S,Down
        else if(keyValue == 31 || keyValue == 26 || keyValue == 82) ui->sBox_color_i1->setValue(ui->sBox_color_i1->value() + 1);//2,W,Up
    }
}

void Widget::keyPressEvent(QKeyEvent *event)//按键按下
{
    if(!isActiveWindow()) return;//若当前为非活动窗口则可能发生切屏 舍弃键值直接返回
    if(event->isAutoRepeat()) return;//若为自动重复触发或正在发送数据则返回
    int key1 = event->key();//读取第一种键值
    int key2 = event->nativeVirtualKey();//读取第二种键值
//    int key3 = event->nativeScanCode();//读取第三种键值(该值能区分左右功能键)
//    printf("key:%d  %d  %d\n", key1, key2, key3);//打印键值
    
    uint8_t keyValue = key_to_USB(key1, key2);//映射到USB键值
    
    if(ifPos){//若正在位置获取
        ifPos = false;
        ui->Bt_pos_get->setStyleSheet(style_mid_black);
        setWindowOpacity(1.0);
        QPoint pt = QCursor::pos();//获取鼠标的绝对位置
        int mouseScreen = QApplication::desktop()->screenNumber(pt);//获取鼠标所在的屏幕
        QRect mouseScreenGeometry = QApplication::desktop()->screen(mouseScreen)->geometry();
        QPoint ptInScreen = pt - mouseScreenGeometry.topLeft();//鼠标在该屏幕中的位置
        ui->spinBox_x->setValue(ptInScreen.x());//填入数据框
        ui->spinBox_y->setValue(ptInScreen.y());
        return;
    }
    
    if(keyValue >= 249 && keyValue <= 252) func |= 0x01 << (keyValue - 249);
    else{
        ui->label_keyValue->setText("USB键值:" + QString::number(keyValue));
        ui->label_keyName->setText("按键:" + USB_to_str(keyValue,0));
        key_num++;
    }
    
    if(keyValue == 249) ui->label_ctrl->setStyleSheet("color: black");
    else if(keyValue == 250) ui->label_shift->setStyleSheet("color: black");
    else if(keyValue == 251) ui->label_alt->setStyleSheet("color: black");
    else if(keyValue == 252) ui->label_win->setStyleSheet("color: black");
    
    keyHandle(keyValue);
    //printf("key:%d\n",keyValue);//打印键值
    //printf("funcP:%d",func);//打印当前功能键
}

void Widget::keyReleaseEvent(QKeyEvent *event)//按键抬起
{
    if(event->isAutoRepeat()){ return; }//若为自动重复触发或正在发送数据则返回
    int key1 = event->key();//读取第一种键值
    int key2 = event->nativeVirtualKey();//读取第二种键值
    
    uint8_t keyValue = key_to_USB(key1,key2);//映射到USB键值
    
    if(keyValue >= 249 && keyValue <= 252){
        if(!ifHold) func &= ~(0x01 << (keyValue - 249));
    }
    else if(key_num > 0) key_num--;
    
    if(key_num == 0){
        ui->label_keyValue->setText("USB键值:None");
        ui->label_keyName->setText("按键:None");
    }
    
    if(!ifHold){
        if(keyValue == 249) ui->label_ctrl->setStyleSheet("color: grey");
        else if(keyValue == 250) ui->label_shift->setStyleSheet("color: grey");
        else if(keyValue == 251) ui->label_alt->setStyleSheet("color: grey");
        else if(keyValue == 252) ui->label_win->setStyleSheet("color: grey");
    }
    keyHandle(keyValue,false);
    //printf("funcR:%d",func);//打印当前功能键
}

void Widget::on_Bt_glb_key_flt_clicked()//按键滤波
{
    bool ifOK = false;
    int ansNum = QInputDialog::getInt(this, "按键滤波", "参数设置(0-100)",
                                      2, 0, 100, 1,//默认值,最小值,最大值,步进
                                      &ifOK, Qt::WindowCloseButtonHint);
    if(!ifOK) return;//若取消则返回
    uint8_t inBuf[1], outBuf[2];
    inBuf[0] = ansNum;//按键滤波参数
    
    hid_set_para(ui->spinBox_vid->value(), ui->spinBox_pid->value(), 0xFF00);   //HID查找参数设置
    uint8_t ret = hid_send_cmd(CHID_CMD_KEY_FLT, inBuf, outBuf);                //HID按键滤波设置
    
    if(ret != CHID_OK){//若获取失败
        QMessageBox::critical(this, "滤波设置", "HID通信失败\n" + CHID_to_str(ret));
        return;
    }else{//若获取成功
        QString fltInfo = "参数已由" + QString::number(outBuf[0]) 
                          + "修改为" + QString::number(outBuf[1]);
        QMessageBox::information(this, "滤波设置", fltInfo);
    }
}

void Widget::on_Bt_glb_rk_calib_clicked()//摇杆校正
{
    uint16_t adcValue[4];
    
    hid_set_para(ui->spinBox_vid->value(), ui->spinBox_pid->value(), 0xFF00);   //HID查找参数设置
    uint8_t ret = hid_send_cmd(CHID_CMD_RK_ADC, NULL, (uint8_t*)adcValue);      //HID获取摇杆ADC值
    
    if(ret != CHID_OK){//若获取失败
        QMessageBox::critical(this, "摇杆校正", "HID通信失败\n" + CHID_to_str(ret));
        return;
    }else{//若获取成功
        QString adcInfo = "中位值由:" + QString::number(adcValue[0]) 
                               + "," + QString::number(adcValue[1])
                        + "\n更新为:" + QString::number(adcValue[2])
                               + "," + QString::number(adcValue[3]);
        QMessageBox::information(this, "摇杆校正", adcInfo);
    }
}

void Widget::on_Bt_glb_ec_freq_clicked()//旋钮倍频
{
    bool ifOK = false;
    int ansNum = QInputDialog::getInt(this, "旋钮倍频","倍频参数(0-1)",
                                      0, 0, 99, 1,//默认值,最小值,最大值,步进
                                      &ifOK, Qt::WindowCloseButtonHint);
    if(!ifOK) return;//若取消则返回
    uint8_t inBuf[2], outBuf[4];
    inBuf[0] = ansNum % 10;//旋钮1倍频参数
    inBuf[1] = (ansNum % 100) / 10;//旋钮2倍频参数
    
    hid_set_para(ui->spinBox_vid->value(), ui->spinBox_pid->value(), 0xFF00);   //HID查找参数设置
    uint8_t ret = hid_send_cmd(CHID_CMD_EC_FREQ, inBuf, outBuf);                //HID旋钮倍频设置
    
    if(ret != CHID_OK){//若获取失败
        QMessageBox::critical(this, "倍频设置", "HID通信失败\n" + CHID_to_str(ret));
        return;
    }else{//若获取成功
        QString fltInfo = "参数已由" + QString("%1").arg(outBuf[0] + 10*outBuf[1], 2, 10, QChar('0')) 
                          + "修改为" + QString("%1").arg(outBuf[2] + 10*outBuf[3], 2, 10, QChar('0'));//格式化为2位数
        QMessageBox::information(this, "倍频设置", fltInfo);
    }
}

void Widget::on_Bt_info_clicked()//信息读取
{
    hid_set_para(ui->spinBox_vid->value(), ui->spinBox_pid->value(), 0xFF00);   //HID查找参数设置
    
    bool ifOK = false;
    int ansNum = QInputDialog::getInt(this, "信息读取", "0-序列号读取\n1-固件版本读取\n2-参数读取\n3-诊断信息读取",
                                      0, 0, 3, 1,//默认值,最小值,最大值,步进
                                      &ifOK, Qt::WindowCloseButtonHint);
    if(!ifOK) return;
    
    uint8_t ret = CHID_OK;
    
    if(ansNum == 0){//序列号读取
        uint16_t outBuf[3] = {0, 0, 0};
        ret = hid_send_cmd(CHID_CMD_UUID, NULL, (uint8_t*)outBuf);
        if(ret != CHID_OK){//若失败
            QMessageBox::critical(this, "序列号读取", "HID通信失败\n" + CHID_to_str(ret));
            return;
        }
        else{//成功
            QMessageBox::information(this, "序列号读取", QString::asprintf("序列号 %04X%04X%04X",
                                                            outBuf[0], outBuf[1], outBuf[2]));
        }
    }
    else if(ansNum == 1){//固件版本读取
        uint8_t outBuf[4] = {0, 0, 0, 0};
        ret = hid_send_cmd(CHID_CMD_FW_VER, NULL, outBuf);
        if(ret != CHID_OK){//若失败
            QMessageBox::critical(this, "固件版本读取", "HID通信失败\n" + CHID_to_str(ret));
            return;
        }
        else{//成功
            QMessageBox::information(this, "固件版本读取", QString::asprintf("版本 V%d.%d.%d.%d", 
                                                            outBuf[0], outBuf[1], outBuf[2], outBuf[3]));
        }
    }
    else if(ansNum == 2){//参数读取
        uint16_t outBuf[4] = {0, 0, 0, 0};
        ret = hid_send_cmd(CHID_CMD_PARA, NULL, (uint8_t*)outBuf);
        if(ret != CHID_OK){//若失败
            QMessageBox::critical(this, "参数读取", "HID通信失败\n" + CHID_to_str(ret));
            return;
        }
        else{//成功
            QMessageBox::information(this, "参数读取", QString::asprintf("摇杆中位 %d,%d\n按键滤波 %d\n旋钮倍频 %d", 
                                                            outBuf[0], outBuf[1], outBuf[2], outBuf[3]));
        }
    }
    else if(ansNum == 3){//诊断信息读取
        uint16_t outBuf[29] = {0};
        ret = hid_send_cmd(CHID_CMD_DIAG, NULL, (uint8_t*)outBuf);
        if(ret != CHID_OK){//若失败
            QMessageBox::critical(this, "诊断信息读取", "HID通信失败\n" + CHID_to_str(ret));
            return;
        }
        else{//成功
            QString diagStr = QString::asprintf("标志错误 %04X        校验错误 %04X\n", outBuf[0], outBuf[1]);
            diagStr += QString::asprintf("看门狗重启 %04X    连续看门狗重启 %04X\n", outBuf[4], outBuf[5]);
            diagStr += "配置数据错误\n";
            for(int i = 0; i < 8; i++) diagStr += QString::asprintf("%04X ", outBuf[8 + i]);
            diagStr += "\n灯效数据错误\n";
            for(int i = 0; i < 8; i++) diagStr += QString::asprintf("%04X ", outBuf[16 + i]);
            diagStr += QString::asprintf("\n最后错误位置 %02X", outBuf[28] & 0xFF);
            QMessageBox::information(this, "诊断信息读取", diagStr);
        }
    }
    
}

void Widget::on_Bt_special_clicked()//特殊功能
{
    hid_set_para(ui->spinBox_vid->value(), ui->spinBox_pid->value(), 0xFF00);   //HID查找参数设置
    
    int intOK = QMessageBox::question(this, "特殊功能", "请务必阅读文档\n再使用这些功能", QMessageBox::Ok, QMessageBox::Cancel);
    if(intOK != QMessageBox::Ok) return;
    
    bool ifOK = false;
    int ansNum = QInputDialog::getInt(this, "特殊功能", "0-软复位\n1-Boot预跳转",
                                      0, 0, 1, 1,//默认值,最小值,最大值,步进
                                      &ifOK, Qt::WindowCloseButtonHint);
    if(!ifOK) return;
    
    uint8_t ret = CHID_OK;
    
    if(ansNum == 0){//软复位
        ret = hid_send_cmd(CHID_CMD_RST, NULL, NULL);
        if(ret != CHID_OK){//若失败
            QMessageBox::critical(this, "软复位", "HID通信失败\n" + CHID_to_str(ret));
            return;
        }
    }
    else if(ansNum == 1){//Boot预跳转
        ret = hid_send_cmd(CHID_CMD_BOOT, NULL, NULL);
        if(ret != CHID_OK){//若失败
            QMessageBox::critical(this, "Boot预跳转", "HID通信失败\n" + CHID_to_str(ret));
            return;
        }
    }
    
}

void Widget::openCfgFile()//打开配置文件
{
    if(!read_cfg_file(fileNow,cfgUnit->cfg_data,512)){
        QMessageBox::critical(this,"提示","无路径");
        return;
    }
    
    if(cfgUnit->read_cfg_data()){
        QFileInfo fileinfo = QFileInfo(fileNow);//文件信息
        ui->lineEdit_fileName->setText(fileinfo.fileName());//显示文件名
    }
    else QMessageBox::critical(this,"提示","配置读取失败");
}

void Widget::saveCfgFile()//保存配置文件
{
    QFileInfo fileinfo = QFileInfo(fileNow);//文件信息
    if(fileinfo.fileName().isEmpty()) return;
    if(fileinfo.suffix().isEmpty()){
        fileNow += ".etcfg";//若无后缀则添加
        fileinfo = QFileInfo(fileNow);
    }
    
    if(cfgUnit->write_cfg_data()){
        write_cfg_file(fileNow,cfgUnit->cfg_data,512);
        ui->lineEdit_fileName->setText(fileinfo.fileName());//显示文件名
    }
    else QMessageBox::critical(this,"提示","配置超长");
}

void Widget::openKeyFile()//打开key文件
{
    if(!read_cfg_file(fileNowKey,cfgUnit->cfg_data_key,257)){//若因无路径而读取失败
        QMessageBox::critical(this,"提示","无路径");
        return;
    }
    QFileInfo fileinfo = QFileInfo(fileNowKey);//文件信息
    ui->lineEdit_fileName_key->setText(fileinfo.fileName());//显示文件名
    if(button_choose == 0 || cfgUnit->get_key_mode(button_choose - 1) != m3_group) return;//未选按键或按键不是按键组模式
    
    //cfgUnit->cbox_k[button_choose - 1]->setCurrentIndex(m3_group);//直接设为按键组模式
    cfgUnit->clear_key(button_choose - 1);//先清空
    if(cfgUnit->read_mode3_data(button_choose - 1)){//若解析并填入成功
        cfgUnit->set_mode3_txt(button_choose - 1);//显示按键组文本
    }
    else QMessageBox::critical(this,"提示","按键配置失败");
    
    cfgUnit->bt_k[button_choose - 1]->setStyleSheet(style_big_black);
    button_choose = 0;
}

void Widget::saveKeyFile()//保存key文件
{
    if(button_choose == 0 || cfgUnit->get_key_mode(button_choose - 1) != m3_group 
            || cfgUnit->cfg_key[button_choose - 1].data.isEmpty()){
        QMessageBox::information(this,"提示","需要选中按键组模式的非空按键");
        return;
    }
    QFileInfo fileinfo = QFileInfo(fileNowKey);//文件信息
    if(fileinfo.fileName().isEmpty()) return;
    if(fileinfo.suffix().isEmpty()){
        fileNowKey += ".etkey";//若无后缀则添加
        fileinfo = QFileInfo(fileNowKey);
    }
    
    if(cfgUnit->write_mode3_data(button_choose - 1)){
        write_cfg_file(fileNowKey,cfgUnit->cfg_data_key,257);
        ui->lineEdit_fileName_key->setText(fileinfo.fileName());//显示文件名
    }
    else QMessageBox::critical(this,"提示","配置储存失败");
    button_choose = 0;
}

void Widget::on_Bt_write_clicked()//写入设备按钮
{
    ui->Bt_write->setStyleSheet(style_big_gray);
    ui->Bt_write->setEnabled(false);//暂时禁用按钮
    if(cfgUnit->write_cfg_data()){
        uint8_t cfgPos = ui->cBox_flash->currentIndex();//填入配置存储位置
        hid_set_para(ui->spinBox_vid->value(), ui->spinBox_pid->value(), 0xFF00);   //HID查找参数设置
        uint8_t ret = hid_send_data(CHID_CMD_CFG_KEY, &cfgPos, cfgUnit->cfg_data);  //HID发送数据
        if(ret == CHID_OK) ;//QMessageBox::information(this,"提示","写入成功");
        else QMessageBox::critical(this, "配置写入", "HID通信失败\n" + CHID_to_str(ret));
    }
    ui->Bt_write->setEnabled(true);//恢复启用按钮
    ui->Bt_write->setStyleSheet(style_big_black);
}

void Widget::on_Bt_open_clicked()//打开文件按钮
{
    QString path = "./configFile";
    QDir dir(path);
    if(!dir.exists()) path = "../configFile";//若从文件打开exe则需要回退到根目录
    QString fileNew = QFileDialog::getOpenFileName(this, QStringLiteral("打开配置文件"),
                                                   path, QStringLiteral("配置文件(*etcfg)"));
    if(fileNew.isEmpty()) return;
    fileNow = fileNew;
    openCfgFile();
}

void Widget::on_Bt_save_clicked()//保存文件按钮
{
    QString path = "./configFile";
    QDir dir(path);
    if(!dir.exists()) path = "../configFile";//若从文件打开exe则需要回退到根目录
    if(fileNow.isEmpty()){
        fileNow = QFileDialog::getSaveFileName(this, QStringLiteral("保存配置文件"),
                                               path, QStringLiteral("配置文件(*etcfg)"));
    }
    saveCfgFile();//保存配置文件
}

void Widget::on_Bt_saveas_clicked()//另存为文件按钮
{
    QString path = "./configFile";
    QDir dir(path);
    if(!dir.exists()) path = "../configFile";//若从文件打开exe则需要回退到根目录
    QString fileNew = QFileDialog::getSaveFileName(this, QStringLiteral("另存为配置文件"),
                                                   path, QStringLiteral("配置文件(*etcfg)"));
    if(fileNew.isEmpty()) return;
    fileNow = fileNew;
    saveCfgFile();//保存配置文件
}

void Widget::on_Bt_write_key_clicked()//写入key按钮
{
    if(button_choose == 0){
        QMessageBox::information(this,"提示","请先选中按键");
        return;
    }
    openKeyFile();//打开key文件
}

void Widget::on_Bt_open_key_clicked()//打开key文件按钮
{
    QString path = "./keyFile";
    QDir dir(path);
    if(!dir.exists()) path = "../keyFile";//若从文件打开exe则需要回退到根目录
    QString fileNewKey = QFileDialog::getOpenFileName(this, QStringLiteral("打开按键配置文件"),
                                                      path, QStringLiteral("按键配置文件(*etkey)"));
    if(fileNewKey.isEmpty()) return;
    fileNowKey = fileNewKey;
    openKeyFile();//打开key文件
}

void Widget::on_Bt_save_key_clicked()//保存key文件按钮
{
    QString path = "./keyFile";
    QDir dir(path);
    if(!dir.exists()) path = "../keyFile";//若从文件打开exe则需要回退到根目录
    if(fileNowKey.isEmpty()){
        fileNowKey = QFileDialog::getSaveFileName(this, QStringLiteral("保存按键配置文件"),
                                                  path, QStringLiteral("按键配置文件(*etkey)"));
    }
    saveKeyFile();//保存key文件
}

void Widget::on_Bt_saveas_key_clicked()//另存为key文件按钮
{
    QString path = "./keyFile";
    QDir dir(path);
    if(!dir.exists()) path = "../keyFile";//若从文件打开exe则需要回退到根目录
    QString fileNewKey = QFileDialog::getSaveFileName(this, QStringLiteral("另存为按键配置文件"),
                                                      path, QStringLiteral("按键配置文件(*etkey)"));
    if(fileNewKey.isEmpty()) return;
    fileNowKey = fileNewKey;
    saveKeyFile();//保存key文件
}

void Widget::passPointer()//传递指针
{
    cfgUnit->label_k[0] = ui->lb_key_1;//按键标签
    cfgUnit->label_k[1] = ui->lb_key_2;
    cfgUnit->label_k[2] = ui->lb_key_3;
    cfgUnit->label_k[3] = ui->lb_key_4;
    cfgUnit->label_k[4] = ui->lb_key_5;
    cfgUnit->label_k[5] = ui->lb_key_6;
    cfgUnit->label_k[6] = ui->lb_key_7;
    cfgUnit->label_k[7] = ui->lb_key_8;
    cfgUnit->label_k[8] = ui->lb_key_9;
    cfgUnit->label_k[9] = ui->lb_key_10;
    cfgUnit->label_k[10] = ui->lb_key_11;
    cfgUnit->label_k[11] = ui->lb_key_12;
    cfgUnit->label_k[12] = ui->lb_key_13;
    cfgUnit->label_k[13] = ui->lb_key_14;
    cfgUnit->label_k[14] = ui->lb_key_15;
    cfgUnit->label_k[15] = ui->lb_key_16;
    
    cfgUnit->label_r[0] = ui->lb_r_1;//摇杆按键标签
    cfgUnit->label_r[1] = ui->lb_r_1_0;
    cfgUnit->label_r[2] = ui->lb_r_1_1;
    cfgUnit->label_r[3] = ui->lb_r_1_2;
    cfgUnit->label_r[4] = ui->lb_r_1_3;
//    cfgUnit->label_r[5] = ui->lb_r_2;
//    cfgUnit->label_r[6] = ui->lb_r_2_0;
//    cfgUnit->label_r[7] = ui->lb_r_2_1;
//    cfgUnit->label_r[8] = ui->lb_r_2_2;
//    cfgUnit->label_r[9] = ui->lb_r_2_3;
    
    cfgUnit->label_e[0] = ui->lb_e_1;//旋钮按键标签
    cfgUnit->label_e[1] = ui->lb_e_1_0;
    cfgUnit->label_e[2] = ui->lb_e_1_1;
    cfgUnit->label_e[3] = ui->lb_e_2;
    cfgUnit->label_e[4] = ui->lb_e_2_0;
    cfgUnit->label_e[5] = ui->lb_e_2_1;
    
    cfgUnit->bt_k[0] = ui->key1;//按键按钮
    cfgUnit->bt_k[1] = ui->key2;
    cfgUnit->bt_k[2] = ui->key3;
    cfgUnit->bt_k[3] = ui->key4;
    cfgUnit->bt_k[4] = ui->key5;
    cfgUnit->bt_k[5] = ui->key6;
    cfgUnit->bt_k[6] = ui->key7;
    cfgUnit->bt_k[7] = ui->key8;
    cfgUnit->bt_k[8] = ui->key9;
    cfgUnit->bt_k[9] = ui->key10;
    cfgUnit->bt_k[10] = ui->key11;
    cfgUnit->bt_k[11] = ui->key12;
    cfgUnit->bt_k[12] = ui->key13;
    cfgUnit->bt_k[13] = ui->key14;
    cfgUnit->bt_k[14] = ui->key15;
    cfgUnit->bt_k[15] = ui->key16;
    
    cfgUnit->bt_r[0] = ui->key_r_1;//摇杆按键按钮
    cfgUnit->bt_r[1] = ui->key_r_1_0;
    cfgUnit->bt_r[2] = ui->key_r_1_1;
    cfgUnit->bt_r[3] = ui->key_r_1_2;
    cfgUnit->bt_r[4] = ui->key_r_1_3;
//    cfgUnit->bt_r[5] = ui->key_r_2;
//    cfgUnit->bt_r[6] = ui->key_r_2_0;
//    cfgUnit->bt_r[7] = ui->key_r_2_1;
//    cfgUnit->bt_r[8] = ui->key_r_2_2;
//    cfgUnit->bt_r[9] = ui->key_r_2_3;
    
    cfgUnit->bt_e[0] = ui->key_e_1;//旋钮按键按钮
    cfgUnit->bt_e[1] = ui->key_e_1_0;
    cfgUnit->bt_e[2] = ui->key_e_1_1;
    cfgUnit->bt_e[3] = ui->key_e_2;
    cfgUnit->bt_e[4] = ui->key_e_2_0;
    cfgUnit->bt_e[5] = ui->key_e_2_1;
    
    cfgUnit->cbox_k[0] = ui->cBox_k_1;//按键模式下拉框
    cfgUnit->cbox_k[1] = ui->cBox_k_2;
    cfgUnit->cbox_k[2] = ui->cBox_k_3;
    cfgUnit->cbox_k[3] = ui->cBox_k_4;
    cfgUnit->cbox_k[4] = ui->cBox_k_5;
    cfgUnit->cbox_k[5] = ui->cBox_k_6;
    cfgUnit->cbox_k[6] = ui->cBox_k_7;
    cfgUnit->cbox_k[7] = ui->cBox_k_8;
    cfgUnit->cbox_k[8] = ui->cBox_k_9;
    cfgUnit->cbox_k[9] = ui->cBox_k_10;
    cfgUnit->cbox_k[10] = ui->cBox_k_11;
    cfgUnit->cbox_k[11] = ui->cBox_k_12;
    cfgUnit->cbox_k[12] = ui->cBox_k_13;
    cfgUnit->cbox_k[13] = ui->cBox_k_14;
    cfgUnit->cbox_k[14] = ui->cBox_k_15;
    cfgUnit->cbox_k[15] = ui->cBox_k_16;
    
    cfgUnit->cbox_r[0] = ui->cBox_r_1;//摇杆模式
    cfgUnit->cbox_r_key[0] = ui->cBox_r_key_1;//摇杆按键模式
//    cfgUnit->cbox_r_2 = ui->cBox_r_2;
//    cfgUnit->cbox_r_key_2 = ui->cBox_r_key_2;
    
    cfgUnit->sbox_r[0] = ui->spinBox_r_1;//摇杆死区
    cfgUnit->sbox_v[0] = ui->spinBox_v_1;//摇杆参数
    cfgUnit->sbox_v[1] = ui->spinBox_v_2;//摇杆参数
    cfgUnit->sbox_v[2] = ui->spinBox_v_3;//摇杆参数
    
    cfgUnit->cbox_e[0] = ui->cBox_e_1;//旋钮模式
    cfgUnit->cbox_e_key[0] = ui->cBox_e_key_1;//旋钮按键模式
    cfgUnit->cbox_e[1] = ui->cBox_e_2;
    cfgUnit->cbox_e_key[1] = ui->cBox_e_key_2;
    
    cfgUnit->cbox_rgb_t_on = ui->cBox_rgb_t_on;//指示灯
    cfgUnit->cbox_rgb_t_off = ui->cBox_rgb_t_off;//配置指示时间
    
    cfgUnit->cbox_sign_r = ui->cBox_sign_r;//指示灯映射
    cfgUnit->cbox_sign_g = ui->cBox_sign_g;
    cfgUnit->cbox_sign_b = ui->cBox_sign_b;
    
    cfgUnit->cb_sign_dir_r = ui->cB_sign_dir_r;//指示灯反向
    cfgUnit->cb_sign_dir_g = ui->cB_sign_dir_g;
    cfgUnit->cb_sign_dir_b = ui->cB_sign_dir_b;
    
    
    cfgUnit->sbox_rgb_r = ui->spinBox_rgb_r;//R
    cfgUnit->sbox_rgb_g = ui->spinBox_rgb_g;//G
    cfgUnit->sbox_rgb_b = ui->spinBox_rgb_b;//B
    cfgUnit->sbox_sign_r = ui->spinBox_sign_r;//R
    cfgUnit->sbox_sign_g = ui->spinBox_sign_g;//G
    cfgUnit->sbox_sign_b = ui->spinBox_sign_b;//B
    
    cfgUnit->cb_r_x[0] = ui->cB_r_x_1;//摇杆反向
    cfgUnit->cb_r_y[0] = ui->cB_r_y_1;
    cfgUnit->cb_r_r[0] = ui->cB_r_r_1;//摇杆右旋90度
//    cfgUnit->cb_r_x_2 = ui->cB_r_x_2;
//    cfgUnit->cb_r_y_2 = ui->cB_r_y_2;
//    cfgUnit->cb_r_r_2 = ui->cB_r_r_2;
    cfgUnit->cb_e[0] = ui->cB_e_1;//旋钮反向
    cfgUnit->cb_e[1] = ui->cB_e_2;
    
    cfgUnit->pte_mode3Box = ui->mode3Box;//按键数据组显示框
    
    cfgUnit->sbox_w = ui->spinBox_w;//屏幕宽度
    cfgUnit->sbox_h = ui->spinBox_h;//屏幕高度
    
    cfgUnit->cbox_key_turn = ui->cBox_key_turn;//方向
    cfgUnit->cbox_pri = ui->cBox_pri;//优先级
}


void Widget::keyClickHandle(uint8_t button_cs)//16按钮按下处理
{
    if(button_choose > 0 && button_class == 1){
        cfgUnit->bt_k[button_choose - 1]->setStyleSheet(style_big_black);
        if(button_cs == button_choose && (state || cfgUnit->get_key_mode(button_choose - 1) == m3_group)){
            ui->lb_key_cs->setText("当前选择:None");
            button_class = button_choose = 0;
            //cfgUnit->set_mode3_str("");
            cfgUnit->set_mode3_txt(0xFF);
            ui->mode3_input->setText("输入");
            return;
        }
    }
    state = button_class = 1;
    button_choose = button_cs;
    QString key_cs_str = "当前选择:KEY " + QString::number(button_cs);
    ui->lb_key_cs->setText(key_cs_str);
    if(cfgUnit->get_key_mode(button_cs - 1) == m3_group){//若为按键组模式
        //cfgUnit->set_mode3_str(cfgUnit->cfg_key[button_cs - 1].str);
        cfgUnit->set_mode3_txt(button_cs - 1);
        state = 0;
    }
    else if(cfgUnit->get_key_mode(button_cs - 1) == m7_fast){//若为连点模式
        //cfgUnit->set_mode3_str(cfgUnit->cfg_key[button_cs - 1].str);
        cfgUnit->set_mode3_txt(button_cs - 1);
    }
    else cfgUnit->set_mode3_txt(0xFF);//cfgUnit->set_mode3_str("");
    cfgUnit->bt_k[button_cs - 1]->setStyleSheet(style_big_gray);
    ui->mode3_input->setText("输入");
}
void Widget::on_key1_clicked(){ keyClickHandle(1);    }
void Widget::on_key2_clicked(){ keyClickHandle(2);    }
void Widget::on_key3_clicked(){ keyClickHandle(3);    }
void Widget::on_key4_clicked(){ keyClickHandle(4);    }
void Widget::on_key5_clicked(){ keyClickHandle(5);    }
void Widget::on_key6_clicked(){ keyClickHandle(6);    }
void Widget::on_key7_clicked(){ keyClickHandle(7);    }
void Widget::on_key8_clicked(){ keyClickHandle(8);    }
void Widget::on_key9_clicked(){ keyClickHandle(9);    }
void Widget::on_key10_clicked(){ keyClickHandle(10);    }
void Widget::on_key11_clicked(){ keyClickHandle(11);    }
void Widget::on_key12_clicked(){ keyClickHandle(12);    }
void Widget::on_key13_clicked(){ keyClickHandle(13);    }
void Widget::on_key14_clicked(){ keyClickHandle(14);    }
void Widget::on_key15_clicked(){ keyClickHandle(15);    }
void Widget::on_key16_clicked(){ keyClickHandle(16);    }

void Widget::keyRkClickHandle(uint8_t button_cs)//摇杆页10按钮按下处理
{
    if(button_choose > 0 && button_class == 2){
        cfgUnit->bt_r[button_choose - 1]->setStyleSheet(style_big_black);
        if(state && button_cs == button_choose){
            button_class = button_choose = 0;
            return;
        }
    }
    state = 1;
    button_class = 2;
    button_choose = button_cs;
    cfgUnit->bt_r[button_cs - 1]->setStyleSheet(style_big_gray);
}
void Widget::on_key_r_1_clicked(){  keyRkClickHandle(1);   }//摇杆1按键
void Widget::on_key_r_1_0_clicked(){  keyRkClickHandle(2);   }//摇杆1上
void Widget::on_key_r_1_1_clicked(){  keyRkClickHandle(3);   }//摇杆1下
void Widget::on_key_r_1_2_clicked(){  keyRkClickHandle(4);   }//摇杆1左
void Widget::on_key_r_1_3_clicked(){  keyRkClickHandle(5);   }//摇杆1右
//void Widget::on_key_r_2_clicked(){  keyRkClickHandle(6);   }//摇杆2按键
//void Widget::on_key_r_2_0_clicked(){  keyRkClickHandle(7);   }//摇杆2上
//void Widget::on_key_r_2_1_clicked(){  keyRkClickHandle(8);   }//摇杆2下
//void Widget::on_key_r_2_2_clicked(){  keyRkClickHandle(9);   }//摇杆2左
//void Widget::on_key_r_2_3_clicked(){  keyRkClickHandle(10);   }//摇杆2右

void Widget::keyEcClickHandle(uint8_t button_cs)//旋钮页6按钮按下处理
{
    if(button_choose > 0 && button_class == 3){
        cfgUnit->bt_e[button_choose - 1]->setStyleSheet(style_big_black);
        if(state && button_cs == button_choose){
            button_class = button_choose = 0;
            return;
        }
    }
    state = 1;
    button_class = 3;
    button_choose = button_cs;
    cfgUnit->bt_e[button_cs - 1]->setStyleSheet(style_big_gray);
}
void Widget::on_key_e_1_clicked(){  keyEcClickHandle(1);   }//旋钮1按键
void Widget::on_key_e_1_0_clicked(){  keyEcClickHandle(2);   }//旋钮1逆时针
void Widget::on_key_e_1_1_clicked(){  keyEcClickHandle(3);   }//旋钮1顺时针
void Widget::on_key_e_2_clicked(){  keyEcClickHandle(4);   }//旋钮2按键
void Widget::on_key_e_2_0_clicked(){  keyEcClickHandle(5);   }//旋钮2逆时针
void Widget::on_key_e_2_1_clicked(){  keyEcClickHandle(6);   }//旋钮2顺时针

void Widget::on_Bt_hold_clicked()//按键保持
{
    if(ifHold){//若之前在按键保持
        ifHold = false;
        func = 0;//清空功能键
        key_num = 0;//清零按键计数
        ui->label_ctrl->setStyleSheet("color: gray");
        ui->label_shift->setStyleSheet("color: gray");
        ui->label_alt->setStyleSheet("color: gray");
        ui->label_win->setStyleSheet("color: gray");
        ui->Bt_hold->setText("按键保持");
    }
    else{
        ifHold = true;
        ui->Bt_hold->setText("取消保持");
    }
}

void Widget::on_mode3Box_cursorPositionChanged(){//文本框光标位置改变
    ui->mode3Box->clearFocus();//取消焦点
    if(cfgUnit->m3boxTextFlag){
        cfgUnit->m3boxTextFlag = 0;//修改文本标志位置位
        return;
    }
//    static int posOld = 0;
    int pos = ui->mode3Box->textCursor().position();//获取光标位置
    
    //printf("cc%d,%d.",posOld,pos);
    
    if(button_choose && button_class == 1){
        if(cfgUnit->get_key_mode(button_choose - 1) == m3_group){
            cfgUnit->set_mode3_cursor(button_choose - 1, pos);//设置模式3光标
        }
    }
//    posOld = pos;
}

void Widget::on_mode3Box_textChanged(){//文本框文本改变
    ui->mode3Box->clearFocus();//取消焦点
    QTextDocument *document = ui->mode3Box->document();
    if(document->isUndoAvailable()) document->undo();//若为手动输入则撤销
}

void Widget::on_mode3_input_clicked(){//按键组输入按钮
    if(ui->mode3_input->text() == "输入"){
        ui->mode3_input->setText("停止");
        ui->mode3_shortcut->setText("快捷键");
        state = 3;
    }else{
        ui->mode3_input->setText("输入");
        state = 0;
    }
}

void Widget::on_mode3_shortcut_clicked(){//按键组快捷键按钮
    if(state == 3){
        ui->mode3_shortcut->setText("单键");
        state = 2;
    }else if(state == 2){
        ui->mode3_shortcut->setText("快捷键");
        state = 3;
    }
}

void Widget::on_mode3_del_clicked(){//按键组删除按钮
    if(button_choose && button_class == 1){
        if(cfgUnit->get_key_mode(button_choose - 1) == m3_group){
            cfgUnit->del_mode3_key(button_choose - 1);
        }
    }
}

void Widget::on_mode3_clear_clicked(){//按键组清空按钮
    if(button_choose && button_class == 1){
        if(cfgUnit->get_key_mode(button_choose - 1) == m3_group){
            cfgUnit->clear_key(button_choose - 1);
        }
    }
}

void Widget::on_key_clear_clicked()//按键清除键
{
    if(button_choose && button_class == 1){
        if(cfgUnit->get_key_mode(button_choose - 1) != m3_group){
            cfgUnit->clear_key(button_choose - 1);
        }
    }
}

void Widget::on_key_clear_r_1_clicked()//摇杆按键清除键
{
    if(button_choose && button_class == 2){
        cfgUnit->clear_rk_key(button_choose - 1);
    }
}

void Widget::on_key_clear_e_1_clicked()//旋钮按键清除键
{
    if(button_choose && button_class == 3){
        cfgUnit->clear_ec_key(button_choose - 1);
    }
}

void Widget::on_key_clear_e_2_clicked()//旋钮按键清除键
{
    if(button_choose && button_class == 3){
        cfgUnit->clear_ec_key(button_choose - 1);
    }
}

void Widget::on_tabWidget_currentChanged(int index)//翻页
{
    State_to_zero();//状态归零
}

void Widget::State_to_zero()//状态归零
{
    ui->lb_key_cs->setText("当前选择:None");
    ui->mode3_input->setText("输入");
    if(button_class == 1){
        cfgUnit->bt_k[button_choose - 1]->setStyleSheet(style_big_black);
    }
    else if(button_class == 2){
        cfgUnit->bt_r[button_choose - 1]->setStyleSheet(style_big_black);
    }
    else if(button_class == 3){
        cfgUnit->bt_e[button_choose - 1]->setStyleSheet(style_big_black);
    }
    button_class = button_choose = 0;
    key_num = 0;
}

void Widget::on_Bt_mouse_L_clicked(){    keyHandle(kv_mouse_l);   }//鼠标左键
void Widget::on_Bt_mouse_M_clicked(){    keyHandle(kv_mouse_m);   }//鼠标中键
void Widget::on_Bt_mouse_R_clicked(){    keyHandle(kv_mouse_r);   }//鼠标右键

void Widget::on_Bt_wheel_up_clicked(){    keyHandle(kv_wheel_up);   }//鼠标滚轮向上
void Widget::on_Bt_wheel_down_clicked(){    keyHandle(kv_wheel_down);   }//鼠标滚轮向下

void Widget::on_Bt_vol_up_clicked(){    keyHandle(kv_vol_up);   }//媒体音量加
void Widget::on_Bt_vol_down_clicked(){    keyHandle(kv_vol_down);   }//媒体音量减
void Widget::on_Bt_vol_mute_clicked(){    keyHandle(kv_vol_mute);   }//媒体静音
void Widget::on_Bt_vol_stop_clicked(){    keyHandle(kv_vol_stop);   }//媒体停止
void Widget::on_Bt_vol_next_clicked(){    keyHandle(kv_vol_next);   }//媒体下一个
void Widget::on_Bt_vol_prev_clicked(){    keyHandle(kv_vol_prev);   }//媒体上一个

void Widget::on_Bt_empty_clicked(){    keyHandle(0);   }//空按键



void Widget::on_Bt_pos_get_clicked()//位置捕获
{
    //State_to_zero();//状态归零
    setWindowOpacity(0.7);//改为半透明
    ui->Bt_pos_get->setStyleSheet(style_mid_gray);
    ifPos = true;
}

void Widget::on_Bt_pos_input_clicked()//位置填入
{
    uint16_t x = ui->spinBox_x->value();
    uint16_t y = ui->spinBox_y->value();
    uint16_t w = ui->spinBox_w->value();
    uint16_t h = ui->spinBox_h->value();
    if(x >= w || y >= h){
        QMessageBox::information(this,"提示","坐标XY应分别小于屏幕长宽");
        return;
    }
    if(state > 0 && button_class == 1){//键盘
        if(cfgUnit->get_key_mode(button_choose - 1) == m3_group){
            if(!cfgUnit->add_mode3_pos(button_choose - 1,x,y)){
                QMessageBox::critical(this,"提示","此键可用空间不足");
            }
        }
        else if(cfgUnit->get_key_mode(button_choose - 1) == m4_move
                || cfgUnit->get_key_mode(button_choose - 1) == m5_click){
            cfgUnit->set_mode4_key(button_choose - 1,x,y);
            cfgUnit->bt_k[button_choose - 1]->setStyleSheet(style_big_black);
            state = 0;
        }
    }
}

void Widget::on_Bt_delay_clicked()//时间填入
{
    if(state > 0 && button_class == 1){//键盘
        if(cfgUnit->get_key_mode(button_choose - 1) == m3_group){
            if(!cfgUnit->add_mode3_delay(button_choose - 1,ui->spinBox_delay->value())){
                QMessageBox::critical(this,"提示","此键可用空间不足");
            }
        }
        else if(cfgUnit->get_key_mode(button_choose - 1) == m7_fast){
            cfgUnit->set_mode7_time(button_choose - 1,ui->spinBox_delay->value());
            cfgUnit->bt_k[button_choose - 1]->setStyleSheet(style_big_black);
            state = 0;
        }
    }
}

void Widget::on_mode3_loop_clicked()
{
    if(!(state > 0 && button_class == 1)) return;//键盘
    if(!(cfgUnit->get_key_mode(button_choose - 1) == m3_group)) return;
    
    int ansSpecial = QMessageBox::question(this,"特殊","插入循环或报文控制?","循环","报文控制",0,-1);
    if(ansSpecial){//报文控制
        int ansEnd = !QMessageBox::question(this,"报文控制","作为报文中止?","是","否","",1,-1);
        int ansGap = !QMessageBox::question(this,"报文控制","与下次报文之间以空报文隔开?","是","否","",1,-1);
        cfgUnit->add_mode3_report(button_choose - 1,(ansEnd << 1) | ansGap);
    }
    else{//循环
        int ansEnd = QMessageBox::question(this,"循环","起始或结束?","起始","结束","",1,-1);
        if(!ansEnd){
            if(!cfgUnit->add_mode3_loop(button_choose - 1,0x02,0)){//增加模式3循环
                QMessageBox::critical(this,"提示","此键可用空间不足");
            }
            return;
        }
        bool ifOK = false;
        int ansNum = QInputDialog::getInt(this,"循环","循环次数(0为无限)",
                                        0,0,65534,1,//默认值,最小值,最大值,步进
                                        &ifOK,Qt::WindowCloseButtonHint);
        if(!ifOK) return;
        int ansAuto = QMessageBox::question(this,"循环","循环方式?","非自动","自动",0,-1);
        if(!cfgUnit->add_mode3_loop(button_choose - 1,ansAuto,ansNum)){//增加模式3循环
            QMessageBox::critical(this,"提示","此键可用空间不足");
        }
    }
}

void Widget::on_spinBox_rgb_r_valueChanged(int arg1){   sys_rgb_display();  }
void Widget::on_spinBox_rgb_g_valueChanged(int arg1){   sys_rgb_display();  }
void Widget::on_spinBox_rgb_b_valueChanged(int arg1){   sys_rgb_display();  }

void Widget::sys_rgb_display(){//显示rgb
    uint8_t vR = ui->spinBox_rgb_r->value(), vG = ui->spinBox_rgb_g->value(), vB = ui->spinBox_rgb_b->value();
    ui->lb_rgb_pic_0->setPixmap(rgbToPix(vR, vG, vB, 30, 30, 0));//图片添加到标签控件
    ui->lb_rgb_pic_1->setPixmap(rgbToPix(vR, vG, vB, 30, 30, 1));//图片添加到标签控件
}














