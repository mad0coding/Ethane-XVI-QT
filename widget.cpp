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
    
    hidApi = new HidApi();
    hidDev = new HidDevice();//HID设备
    
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

void Widget::key_handle(uint8_t keyValue, bool ifPress = true)//按键处理
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
                int ansTP = QMessageBox::question(this,"切换配置","切换方式?","临时","永久",0,-1);
                int ansTO = QMessageBox::question(this,"切换配置","切换目标?","1","2","3",0,-1);
                if(ansTP == 0){//临时切换
                    cfgUnit->set_mode6_key(button_choose - 1,keyValue,(ansTO + 1) | 0x08);
                }
                else{//永久切换
                    cfgUnit->set_mode6_key(button_choose - 1,0,ansTO + 1);
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
//            cfgUnit->set_rk_key(button_choose - 1,keyValue);
//            cfgUnit->bt_r[button_choose - 1]->setStyleSheet(style_big_black);
//            state = 0;
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
    if(event->isAutoRepeat() || ifSending){ return; }//若为自动重复触发或正在发送数据则返回
    int key1 = event->key();//读取第一种键值
    int key2 = event->nativeVirtualKey();//读取第二种键值
//    int key3 = event->nativeScanCode();//读取第三种键值(该值能区分左右功能键)
//    printf("key:%d  %d  %d\n",key1,key2,key3);//打印键值
    
    uint8_t keyValue = key_to_USB(key1,key2);//映射到USB键值
    
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
    
    key_handle(keyValue);
    //printf("key:%d\n",keyValue);//打印键值
    //printf("funcP:%d",func);//打印当前功能键
}

void Widget::keyReleaseEvent(QKeyEvent *event)//按键抬起
{
    if(event->isAutoRepeat() || ifSending){ return; }//若为自动重复触发或正在发送数据则返回
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
    key_handle(keyValue,false);
    //printf("funcR:%d",func);//打印当前功能键
}

uint8_t Widget::hid_find_open(uint16_t findVid, uint16_t findPid, uint16_t findUsagePage)//HID设备查找并打开
{
    HidDeviceList devList;//HID设备列表
    if(!hidApi->isInitialized()) return CHID_ERR_INIT;//HidApi未成功初始化则退出
    devList = hidApi->scanDevices(findVid, findPid);//按VID和PID扫描设备
    int findDevNum = 0;//设备计数
    for(size_t i = 0; i < devList.size(); i++){//在VID和PID符合的设备中进一步查找
        if(devList[i].getUsagePage() == findUsagePage){//匹配UsagePage 实际主要是为了匹配端点
            if(findDevNum > 0) return CHID_MULTI_DEV;//若有多个设备则退出
            findDevNum++;//设备计数
            *hidDev = devList[i];//选定该设备
        }
    }
    if(findDevNum == 0) return CHID_NO_DEV;//未找到设备则退出
    //if(!hidDev->isInitialized()) return CHID_ERR_INIT;//HID设备未初始化则退出
    if(!hidDev->open()) return CHID_ERR_OPEN;//HID设备打开失败则退出
    return CHID_OK;
}

uint8_t Widget::hid_close()//HID设备关闭
{
    return (hidDev->close()) ? CHID_OK : CHID_ERR_CLOSE;//关闭HID设备
}

uint8_t Widget::hid_write_read(uint8_t *writeBuf, uint8_t *readBuf)//HID先写后读
{
    std::string hidWriteStr;//创建写入缓存
    hidWriteStr.resize(65, 0);//重置为65字节
    hidWriteStr[0] = 0;//首字节固定为0
    for(int i = 0; i < 64; i++) hidWriteStr[i + 1] = writeBuf[i];//输入数据拷贝
    
    while(hidDev->readAvailable()) hidDev->read(1);//若已有数据则先读出
    if(hidDev->write(hidWriteStr) != 65) return CHID_ERR_WRITE;//若写入失败则退出
    
    std::string hidReadStr = hidDev->read(500);//等待读取响应数据最多500ms
    if(hidReadStr.length() != 65) return CHID_ERR_READ;//若读取失败则退出
    for(int i = 0; i < 64; i++) readBuf[i] = hidReadStr[i + 1];//输出数据拷贝
    return CHID_OK;
}

uint8_t Widget::connectHID(uint8_t cmd, uint8_t *outBuf)//以自定义HID连接设备
{
    uint8_t ret = CHID_OK;
    ret = hid_find_open(ui->spinBox_vid->value(), ui->spinBox_pid->value(), 0xFF00);//HID设备查找并打开
    if(ret != CHID_OK){//失败
        return ret;
    }
    
    uint8_t writeBuf[64], readBuf[64];//读写缓存

    if(cmd == 0 || cmd == 1){//配置或灯效连接命令
        if(cmd == 0){
            memcpy(writeBuf, "CH", 2);//填入配置连接指令
            writeBuf[2] = '1' + ui->cBox_flash->currentIndex();//填入配置存储位置
        }
        else{
            memcpy(writeBuf, "LT", 2);//填入灯效连接指令
            writeBuf[2] = '1' + ui->cBox_flash_color->currentIndex();//填入灯效存储位置
        }
        
        ret = hid_write_read(writeBuf, readBuf);//HID先写后读
        if(ret != CHID_OK){//失败
            hid_close();//HID设备关闭
            return ret;
        }

        if(readBuf[0] == 'R' && readBuf[1] == writeBuf[0] && readBuf[2] == writeBuf[1]){//若正确响应
            return CHID_OK;
        }
    }
    else if(cmd == 2){//摇杆校正命令
        memcpy(writeBuf, "RKC", 3);//填入校正命令
        
        ret = hid_write_read(writeBuf, readBuf);//HID先写后读
        hid_close();//HID设备关闭
        if(ret != CHID_OK) return ret;//失败
        
        if(readBuf[0] == 'R' && readBuf[1] == 'K'){//若正确响应
            uint16_t adcValue[4];
            adcValue[0] = (readBuf[2] << 8) | readBuf[3];//获取ADC值
            adcValue[1] = (readBuf[4] << 8) | readBuf[5];
            adcValue[2] = (readBuf[6] << 8) | readBuf[7];
            adcValue[3] = (readBuf[8] << 8) | readBuf[9];
            if(adcValue[2] > 4095 || adcValue[3] > 4095) return 1;//数据错误
            if(outBuf){
                memcpy(outBuf, adcValue, sizeof(adcValue));
            }
            return CHID_OK;
        }
    }
    else if(cmd == 3){//修改按键滤波参数命令
        memcpy(writeBuf, "KYF", 3);//填入修改命令
        writeBuf[3] = ui->sBox_key_flt->value();//填入按键滤波参数
        
        ret = hid_write_read(writeBuf, readBuf);//HID先写后读
        hid_close();//HID设备关闭
        if(ret != CHID_OK) return ret;//失败
        
        if(readBuf[0] == 'K' && readBuf[1] == 'Y'){//若正确响应
            if(outBuf){
                memcpy(outBuf, &readBuf[2], 2);
            }
            return CHID_OK;
        }
    }
    hid_close();//HID设备关闭
    return CHID_BAD_REP;//设备无响应或错误响应
}

bool Widget::writeHID(uint8_t mode, uint8_t *buf)//以自定义HID向设备写入数据
{
    uint8_t ret = CHID_OK;
    uint8_t writeBuf[64], readBuf[64];//读写缓存
    
    ifSending = true;//正在发送
    int success = 0, packNum = (mode == 0) ? 8 : 4;//包数
    ret = connectHID(mode);//连接
    if(ret == CHID_OK){//若连接成功
        for(int i = 0; i < packNum; i++){//64*8=512字节
            mySleep(5);//强制延时5ms防止HID发送卡死
            
            memcpy(writeBuf, (buf + i * 64), 64);//拷贝到写入缓存
            
            ret = hid_write_read(writeBuf, readBuf);//HID先写后读
            if(ret != CHID_OK) break;//失败
            
            if(readBuf[0] == i && readBuf[1] == 'C' && readBuf[2] == 'H'){//若正确响应
                success++;
            }
        }
    }
    hid_close();//HID设备关闭
    
    ifSending = false;//发送结束
    
    if(success == packNum) return CHID_OK;//成功个数等于包个数
    else return ret;//失败
}

void Widget::on_Bt_glb_key_flt_clicked()//按键滤波
{
    ifSending = true;//正在发送
    uint8_t outBuf[2];
    uint8_t ret = connectHID(3, outBuf);//按键滤波设置
    ifSending = false;//发送结束
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
    ifSending = true;//正在发送
    uint16_t adcValue[4];
    uint8_t ret = connectHID(2, (uint8_t*)adcValue);//获取摇杆ADC值
    ifSending = false;//发送结束
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

void Widget::on_Bt_glb_ec_freq_clicked()//旋钮设置
{
    int ansAdj = QMessageBox::question(this,"输入调整","摇杆校正或按键滤波设置?","摇杆校正","滤波设置","取消",2,-1);
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
        uint8_t ret = writeHID(0, cfgUnit->cfg_data);
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
    
    cfgUnit->cbox_key_turn = ui->cBox_key_turn;//方向
    
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
    
    cfgUnit->cbox_wave = ui->cBox_wave;//指示灯呼吸
    cfgUnit->cbox_colorful = ui->cBox_colorful;//指示灯变色
    cfgUnit->cbox_rgb_delay = ui->cBox_rgb_delay;//配置指示时间
    
    cfgUnit->cbox_pri = ui->cBox_pri;//优先级
    
//    cfgUnit->sbox_light = ui->spinBox_light;//灯亮度
//    cfgUnit->sbox_light_rgb = ui->spinBox_light_rgb;//RGB亮度
    
    cfgUnit->sbox_w = ui->spinBox_w;//屏幕宽度
    cfgUnit->sbox_h = ui->spinBox_h;//屏幕高度
    cfgUnit->sbox_rgb_r = ui->spinBox_rgb_r;//R
    cfgUnit->sbox_rgb_g = ui->spinBox_rgb_g;//G
    cfgUnit->sbox_rgb_b = ui->spinBox_rgb_b;//B
    cfgUnit->sbox_rgb_light = ui->spinBox_rgb_light;//RGB亮度
    
    cfgUnit->ckbox_r_x[0] = ui->cB_r_x_1;//摇杆反向
    cfgUnit->ckbox_r_y[0] = ui->cB_r_y_1;
    cfgUnit->ckbox_r_r[0] = ui->cB_r_r_1;//摇杆右旋90度
//    cfgUnit->ckbox_r_x_2 = ui->cB_r_x_2;
//    cfgUnit->ckbox_r_y_2 = ui->cB_r_y_2;
//    cfgUnit->ckbox_r_r_2 = ui->cB_r_r_2;
    cfgUnit->ckbox_e[0] = ui->cB_e_1;//旋钮反向
    cfgUnit->ckbox_e[1] = ui->cB_e_2;
    
    cfgUnit->ckbox_sys_rgb_rk = ui->cB_sys_rgb_rk;//RGB摇杆映射
    cfgUnit->ckbox_sys_rgb_clicker = ui->cB_sys_rgb_clicker;//RGB自动连点指示
    cfgUnit->ckbox_sys_rgb_m3 = ui->cB_sys_rgb_m3;//RGB按键组指示
    
    cfgUnit->pte_mode3Box = ui->mode3Box;//按键数据组显示框
}


void Widget::key_click_handle(uint8_t button_cs)//16按钮按下处理
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
void Widget::on_key1_clicked(){ key_click_handle(1);    }
void Widget::on_key2_clicked(){ key_click_handle(2);    }
void Widget::on_key3_clicked(){ key_click_handle(3);    }
void Widget::on_key4_clicked(){ key_click_handle(4);    }
void Widget::on_key5_clicked(){ key_click_handle(5);    }
void Widget::on_key6_clicked(){ key_click_handle(6);    }
void Widget::on_key7_clicked(){ key_click_handle(7);    }
void Widget::on_key8_clicked(){ key_click_handle(8);    }
void Widget::on_key9_clicked(){ key_click_handle(9);    }
void Widget::on_key10_clicked(){ key_click_handle(10);    }
void Widget::on_key11_clicked(){ key_click_handle(11);    }
void Widget::on_key12_clicked(){ key_click_handle(12);    }
void Widget::on_key13_clicked(){ key_click_handle(13);    }
void Widget::on_key14_clicked(){ key_click_handle(14);    }
void Widget::on_key15_clicked(){ key_click_handle(15);    }
void Widget::on_key16_clicked(){ key_click_handle(16);    }

void Widget::key_r_click_handle(uint8_t button_cs)//摇杆页10按钮按下处理
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
void Widget::on_key_r_1_clicked(){  key_r_click_handle(1);   }//摇杆1按键
void Widget::on_key_r_1_0_clicked(){  key_r_click_handle(2);   }//摇杆1上
void Widget::on_key_r_1_1_clicked(){  key_r_click_handle(3);   }//摇杆1下
void Widget::on_key_r_1_2_clicked(){  key_r_click_handle(4);   }//摇杆1左
void Widget::on_key_r_1_3_clicked(){  key_r_click_handle(5);   }//摇杆1右
//void Widget::on_key_r_2_clicked(){  key_r_click_handle(6);   }//摇杆2按键
//void Widget::on_key_r_2_0_clicked(){  key_r_click_handle(7);   }//摇杆2上
//void Widget::on_key_r_2_1_clicked(){  key_r_click_handle(8);   }//摇杆2下
//void Widget::on_key_r_2_2_clicked(){  key_r_click_handle(9);   }//摇杆2左
//void Widget::on_key_r_2_3_clicked(){  key_r_click_handle(10);   }//摇杆2右

void Widget::key_e_click_handle(uint8_t button_cs)//旋钮页6按钮按下处理
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
void Widget::on_key_e_1_clicked(){  key_e_click_handle(1);   }//旋钮1按键
void Widget::on_key_e_1_0_clicked(){  key_e_click_handle(2);   }//旋钮1逆时针
void Widget::on_key_e_1_1_clicked(){  key_e_click_handle(3);   }//旋钮1顺时针
void Widget::on_key_e_2_clicked(){  key_e_click_handle(4);   }//旋钮2按键
void Widget::on_key_e_2_0_clicked(){  key_e_click_handle(5);   }//旋钮2逆时针
void Widget::on_key_e_2_1_clicked(){  key_e_click_handle(6);   }//旋钮2顺时针

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

void Widget::on_Bt_mouse_L_clicked(){    key_handle(kv_mouse_l);   }//鼠标左键
void Widget::on_Bt_mouse_M_clicked(){    key_handle(kv_mouse_m);   }//鼠标中键
void Widget::on_Bt_mouse_R_clicked(){    key_handle(kv_mouse_r);   }//鼠标右键

void Widget::on_Bt_wheel_up_clicked(){    key_handle(kv_wheel_up);   }//鼠标滚轮向上
void Widget::on_Bt_wheel_down_clicked(){    key_handle(kv_wheel_down);   }//鼠标滚轮向下

void Widget::on_Bt_vol_up_clicked(){    key_handle(kv_vol_up);   }//媒体音量加
void Widget::on_Bt_vol_down_clicked(){    key_handle(kv_vol_down);   }//媒体音量减
void Widget::on_Bt_vol_mute_clicked(){    key_handle(kv_vol_mute);   }//媒体静音
void Widget::on_Bt_vol_stop_clicked(){    key_handle(kv_vol_stop);   }//媒体停止
void Widget::on_Bt_vol_next_clicked(){    key_handle(kv_vol_next);   }//媒体下一个
void Widget::on_Bt_vol_prev_clicked(){    key_handle(kv_vol_prev);   }//媒体上一个

void Widget::on_Bt_empty_clicked(){    key_handle(0);   }//空按键



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






