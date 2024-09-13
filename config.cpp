#include "config.h"

config::config()
{
    cfg_data = new uint8_t[512];
    cfg_data_key = new uint8_t[257];
    cfg_key = new config_key[KEY_NUM];
    rk_key = new uint8_t[RK_NUM * 5];
    ec_key = new uint8_t[EC_NUM * 3];
    rk_key_func = new uint8_t[RK_NUM];
    ec_key_func = new uint8_t[EC_NUM];
    memset(cfg_data,0,512);
    memset(cfg_data_key,0,257);
    memset(rk_key,0,RK_NUM * 5);
    memset(ec_key,0,EC_NUM * 3);
    memset(rk_key_func,0,RK_NUM);
    memset(ec_key_func,0,EC_NUM);
}

void config::set_mode1_key(uint8_t key_i, uint8_t key_val)//设置模式1按键
{
    cfg_key[key_i].key = key_val;
    QString keyName = USB_to_str(key_val,0);
    label_k[key_i]->setText(keyName);
}

void config::set_mode2_key(uint8_t key_i, uint8_t key_val, uint8_t func)//设置模式2按键
{
    cfg_key[key_i].key = key_val;
    cfg_key[key_i].func = func;
    QString keyName = "";
    if(func & 0x01) keyName += "C";//ctrl
    if(func & 0x04) keyName += "A";//alt
    if(func & 0x02) keyName += "S";//shift
    if(func & 0x08) keyName += "W";//win
    keyName += "+" + USB_to_str(key_val,0);
    label_k[key_i]->setText(keyName);
}

bool config::add_mode3_key(uint8_t key_i, uint8_t key_val, uint8_t shift)//增加模式3按键
{
    if(255 - cfg_key[key_i].data.size() < (shift ? 2 : 1)) return false;//超长
    QString keyName = USB_to_str(key_val,shift);
    QVector<uint8_t> vct = {};
    if(shift){
        vct.append(kv_shift);
//        cfg_key[key_i].data.append(kv_shift);
//        cfg_key[key_i].dataNum.append(2);
    }
//    else cfg_key[key_i].dataNum.append(1);
    vct.append(key_val);
//    cfg_key[key_i].data.append(key_val);
//    cfg_key[key_i].str += keyName;
//    cfg_key[key_i].strNum.append(keyName.length());
    add_mode3_unit(key_i, vct, keyName);//增加一个数据单元
//    set_mode3_txt(key_i);//设置模式3显示文本
    return true;
}

bool config::add_mode3_func(uint8_t key_i, uint8_t key_val, uint8_t func)//增加模式3快捷键
{
    if(255 - cfg_key[key_i].data.size() < 3) return false;//超长
    QString keyName = "<";
    QVector<uint8_t> vct = {};
    if(func & 0x01) keyName += "C";//ctrl
    if(func & 0x04) keyName += "A";//alt
    if(func & 0x02) keyName += "S";//shift
    if(func & 0x08) keyName += "W";//win
    keyName += "+" + USB_to_str(key_val,0) + ">";
    vct.append(kv_shortcut);
    vct.append(key_val);
    vct.append(func);
//    cfg_key[key_i].data.append(kv_shortcut);
//    cfg_key[key_i].data.append(key_val);
//    cfg_key[key_i].data.append(func);
//    cfg_key[key_i].str += keyName;
//    cfg_key[key_i].strNum.append(keyName.length());
//    cfg_key[key_i].dataNum.append(3);
    add_mode3_unit(key_i, vct, keyName);//增加一个数据单元
//    set_mode3_txt(key_i);//设置模式3显示文本
    return true;
}

bool config::add_mode3_pos(uint8_t key_i, uint16_t x, uint16_t y)//增加模式3光标移位
{
    if(255 - cfg_key[key_i].data.size() < 5) return false;//超长
    QString keyName = "[" + QString::number(x) + "," + QString::number(y) + "]";
    QVector<uint8_t> vct = {};
    vct.append(kv_point);
    vct.append((uint8_t)((x>>8) & 0xFF));//x高8位
    vct.append((uint8_t)(x & 0xFF));//x低8位
    vct.append((uint8_t)((y>>8) & 0xFF));//y高8位
    vct.append((uint8_t)(y & 0xFF));//y低8位
//    cfg_key[key_i].data.append(kv_point);
//    cfg_key[key_i].data.append((uint8_t)((x>>8) & 0xFF));//x高8位
//    cfg_key[key_i].data.append((uint8_t)(x & 0xFF));//x低8位
//    cfg_key[key_i].data.append((uint8_t)((y>>8) & 0xFF));//y高8位
//    cfg_key[key_i].data.append((uint8_t)(y & 0xFF));//y低8位
//    cfg_key[key_i].str += keyName;
//    cfg_key[key_i].strNum.append(keyName.length());
//    cfg_key[key_i].dataNum.append(5);
    add_mode3_unit(key_i, vct, keyName);//增加一个数据单元
//    set_mode3_txt(key_i);//设置模式3显示文本
    return true;
}

bool config::add_mode3_delay(uint8_t key_i, uint16_t t)//增加模式3延时
{
    if(255 - cfg_key[key_i].data.size() < 3) return false;//超长
    QString keyName = "[" + QString::number(t) + "ms]";
    QVector<uint8_t> vct = {};
    vct.append(kv_delay);
    vct.append((uint8_t)((t>>8) & 0xFF));//高8位
    vct.append((uint8_t)(t & 0xFF));//低8位
//    cfg_key[key_i].data.append(kv_delay);
//    cfg_key[key_i].data.append((uint8_t)((t>>8) & 0xFF));//高8位
//    cfg_key[key_i].data.append((uint8_t)(t & 0xFF));//低8位
//    cfg_key[key_i].str += keyName;
//    cfg_key[key_i].strNum.append(keyName.length());
//    cfg_key[key_i].dataNum.append(3);
    add_mode3_unit(key_i, vct, keyName);//增加一个数据单元
//    set_mode3_txt(key_i);//设置模式3显示文本
    return true;
}

bool config::add_mode3_loop(uint8_t key_i, uint8_t func, uint16_t n)//增加模式3循环
{
    if(255 - cfg_key[key_i].data.size() < 4) return false;//超长
    QString keyName = "[Loop";
    QVector<uint8_t> vct = {};
    if(func == 0x02) keyName += "Start]";//起始符
    else{
        if(func) keyName += "Auto";//自动循环
        keyName += ":" + QString::number(n) + "]";
    }
    vct.append(kv_loop);
    vct.append(func);
    vct.append((uint8_t)((n>>8) & 0xFF));//高8位
    vct.append((uint8_t)(n & 0xFF));//低8位
//    cfg_key[key_i].data.append(kv_loop);
//    cfg_key[key_i].data.append(func);
//    cfg_key[key_i].data.append((uint8_t)((n>>8) & 0xFF));//高8位
//    cfg_key[key_i].data.append((uint8_t)(n & 0xFF));//低8位
//    cfg_key[key_i].str += keyName;
//    cfg_key[key_i].strNum.append(keyName.length());
//    cfg_key[key_i].dataNum.append(4);
    add_mode3_unit(key_i, vct, keyName);//增加一个数据单元
//    set_mode3_txt(key_i);//设置模式3显示文本
    return true;
}

bool config::add_mode3_report(uint8_t key_i, uint8_t func)//增加模式3报文控制
{
    if(255 - cfg_key[key_i].data.size() < 2) return false;//超长
    QString keyName = "[ReportCtrl:";
    QVector<uint8_t> vct = {};
    if(func & 0x02) keyName += "End,";
    else keyName += "NotEnd,";
    if(func & 0x01) keyName += "Gap]";
    else keyName += "NoGap]";
    vct.append(kv_report);
    vct.append(func);
//    cfg_key[key_i].data.append(kv_report);
//    cfg_key[key_i].data.append(func);
//    cfg_key[key_i].str += keyName;
//    cfg_key[key_i].strNum.append(keyName.length());
//    cfg_key[key_i].dataNum.append(2);
    add_mode3_unit(key_i, vct, keyName);//增加一个数据单元
//    set_mode3_txt(key_i);//设置模式3显示文本
    return true;
}

void config::set_mode4_key(uint8_t key_i, uint16_t x, uint16_t y)//设置模式4按键
{
    cfg_key[key_i].x = x;
    cfg_key[key_i].y = y;
    QString keyName = "[" + QString::number(x) + "," + QString::number(y) + "]";
    label_k[key_i]->setText(keyName);
}

void config::set_mode6_key(uint8_t key_i, uint8_t key_val, uint8_t func)//设置模式6按键及切换
{
    if(key_val != 0xFF) cfg_key[key_i].key = key_val;
    if(func != 0xFF) cfg_key[key_i].func = func;
    QString keyName = "";
    if(cfg_key[key_i].func & 0x08){//若为临时切换
        keyName += "T";
        keyName += QString::number(cfg_key[key_i].func & 0x07);
    }
    else{
        keyName += "P";
        keyName += QString::number(cfg_key[key_i].func & 0x07);
    }
    if(cfg_key[key_i].key != 0){
        keyName += USB_to_str(cfg_key[key_i].key,0);
    }
    label_k[key_i]->setText(keyName);
}

void config::set_mode7_key(uint8_t key_i, uint8_t key_val, uint8_t func)//设置模式7按键及配置
{
    if(key_val != 0xFF) cfg_key[key_i].key = key_val;
    if(func != 0xFF) cfg_key[key_i].func = func;
    
    QString keyName = USB_to_str(cfg_key[key_i].key,0);
    label_k[key_i]->setText(keyName);
    
    cfg_key[key_i].str = "间隔:" + QString::number(cfg_key[key_i].t) + "ms\n";
    if(cfg_key[key_i].func){//若为自动连点
        cfg_key[key_i].str += "按一次自动持续连点,再按一次解除";
    }
    else{//非自动连点
        cfg_key[key_i].str += "仅按住时连点";
    }
    pte_mode3Box->setPlainText(cfg_key[key_i].str);//显示到数据框
}

void config::set_mode7_time(uint8_t key_i, uint16_t t)//设置模式7时间
{
    cfg_key[key_i].t = t;
    cfg_key[key_i].str = "间隔:" + QString::number(cfg_key[key_i].t) + "ms\n";
    if(cfg_key[key_i].func){//若为自动连点
        cfg_key[key_i].str += "按一次自动持续连点,再按一次解除";
    }
    else{//非自动连点
        cfg_key[key_i].str += "仅按住时连点";
    }
    pte_mode3Box->setPlainText(cfg_key[key_i].str);//显示到数据框
}

void config::set_rk_key(uint8_t key_i, uint8_t key_val)//设置摇杆按键
{
    QString prefix[5] = {"KEY:","↑:","↓:","←:","→:"};//前缀
    rk_key[key_i] = key_val;
    QString keyName = USB_to_str(key_val,0);
    label_r[key_i]->setText(prefix[key_i % 5] + keyName);
}

void config::set_ec_key(uint8_t key_i, uint8_t key_val)//设置旋钮按键
{
    QString prefix[3] = {"KEY:","↶:","↷:"};//前缀
    ec_key[key_i] = key_val;
    QString keyName = USB_to_str(key_val,0);
    label_e[key_i]->setText(prefix[key_i % 3] + keyName);
}

void config::set_mode3_cursor(uint8_t key_i, uint16_t pos){//设置模式3光标
    int cursor = 0, str_i = 0;
    for(cursor = 0; cursor < cfg_key[key_i].strNum.length(); cursor++){//确定光标位置
        if(str_i + 1 >= pos) break;//出于点击零位问题以及考虑光标符占位所以要让出1字符
        str_i += cfg_key[key_i].strNum.value(cursor);
    }
    cfg_key[key_i].cursor = cursor;//记录光标位置
    set_mode3_txt(key_i);//设置模式3显示文本
//    printf("c:%d,%d.",pos,cursor);
}
void config::set_mode3_txt(uint8_t key_i){//设置模式3显示文本
    if(key_i == 0xFF){
        m3boxTextFlag = 1;//修改文本标志位置位
        pte_mode3Box->setPlainText("");//清空文本框
        return;
    }
    QString str = cfg_key[key_i].str;
    int str_i = 0;
    for(int i = 0; i < cfg_key[key_i].cursor; i++) str_i += cfg_key[key_i].strNum.value(i);
    str.insert(str_i, '|');//插入光标符号
    m3boxTextFlag = 1;//修改文本标志位置位
    pte_mode3Box->setPlainText(str);//显示到文本框
}

void config::add_mode3_unit(uint8_t key_i, QVector<uint8_t> vct, QString str){//增加模式3单元
//    cfg_key[key_i].data.append(vct);
//    cfg_key[key_i].str += str;
//    cfg_key[key_i].strNum.append(str.length());
//    cfg_key[key_i].dataNum.append(vct.length());
    int cursor = cfg_key[key_i].cursor;
    int str_i = 0, data_i = 0;
    for(int i = 0; i < cursor; i++) str_i += cfg_key[key_i].strNum.value(i);//计算文本插入位置
    for(int i = 0; i < cursor; i++) data_i += cfg_key[key_i].dataNum.value(i);//计算数据插入位置
    
    cfg_key[key_i].str.insert(str_i, str);//插入文本字符串
    for(int i = 0; i < vct.length(); i++) cfg_key[key_i].data.insert(data_i + i, vct.value(i));//按数量插入数据

    cfg_key[key_i].strNum.insert(cursor, str.length());//插入文本计数
    cfg_key[key_i].dataNum.insert(cursor, vct.length());//插入数据计数
    
    cfg_key[key_i].cursor++;//光标递增
    
//    printf("a:%d,%d.",cfg_key[key_i].strNum.length(),cfg_key[key_i].cursor);
    set_mode3_txt(key_i);//设置模式3显示文本
}

void config::del_mode3_key(uint8_t key_i)
{
    if(cfg_key[key_i].data.empty() || cfg_key[key_i].cursor == 0) return;//已为空或光标在最前则退出
    int cursor = cfg_key[key_i].cursor;
    
    int str_i = 0, data_i = 0;
    for(int i = 0; i < cursor - 1; i++) str_i += cfg_key[key_i].strNum.value(i);//计算文本删除位置
    for(int i = 0; i < cursor - 1; i++) data_i += cfg_key[key_i].dataNum.value(i);//计算数据删除位置
    
//    cfg_key[key_i].str.chop(cfg_key[key_i].strNum.last());//按数量删除文本
//    for(uint8_t i = 0; i < cfg_key[key_i].dataNum.last(); i++){
//        cfg_key[key_i].data.pop_back();//按数量删除键值数据
//    }
//    cfg_key[key_i].strNum.pop_back();//删除文本计数
//    cfg_key[key_i].dataNum.pop_back();//删除数据计数
    
    cfg_key[key_i].str.remove(str_i, cfg_key[key_i].strNum.value(cursor - 1));//按数量删除文本
    cfg_key[key_i].data.remove(data_i, cfg_key[key_i].dataNum.value(cursor - 1));//按数量删除键值数据
    
    cfg_key[key_i].strNum.remove(cursor - 1);//删除文本计数
    cfg_key[key_i].dataNum.remove(cursor - 1);//删除数据计数
    
    cfg_key[key_i].cursor--;//光标递减
    
//    printf("d:%d,%d.",cfg_key[key_i].strNum.length(),cfg_key[key_i].cursor);
    set_mode3_txt(key_i);//重新设置模式3显示文本
}

void config::clear_key(uint8_t key_i)//清除模式1~3按键
{
    cfg_key[key_i].key = 0;
    cfg_key[key_i].func = 0;
    cfg_key[key_i].str.clear();
    cfg_key[key_i].data.clear();
    cfg_key[key_i].strNum.clear();
    cfg_key[key_i].dataNum.clear();
    cfg_key[key_i].cursor = 0;//光标归零
    label_k[key_i]->setText("[]");//显示清空
    //pte_mode3Box->setPlainText("");
    set_mode3_txt(/*key_i*/0xFF);//设置模式3显示文本为空
}

void config::clear_rk_key(uint8_t key_i)//清除摇杆按键
{
    QString prefix[5] = {"KEY:[]","↑:[]","↓:[]","←:[]","→:[]"};//前缀
    rk_key[key_i] = 0;
    if((key_i % 5) == 0) rk_key_func[key_i / 5] = 0;//清空func
    label_r[key_i]->setText(prefix[key_i % 5]);
}

void config::clear_ec_key(uint8_t key_i)//清除旋钮按键
{
    QString prefix[3] = {"KEY:[]","↶:[]","↷:[]"};//前缀
    ec_key[key_i] = 0;
    if((key_i % 3) == 0) ec_key_func[key_i / 3] = 0;//清空func
    label_e[key_i]->setText(prefix[key_i % 3]);
}

bool config::write_cfg_data()//写入配置数组
{
    uint8_t mode = 0;//模式
    uint16_t len = 0;//长度
    uint16_t i = 0;//访问cfg_data
    
    memset(cfg_data,0,512);
    
    for(uint8_t keyi = 0; keyi < KEY_NUM; keyi++){
        cfg_data[i++] = keyi + 1;//存储id
        mode = get_key_mode(keyi);//获取模式
        cfg_data[i++] = mode;//存储模式
        if(mode == m1_button){
            cfg_data[i++] = cfg_key[keyi].key;//存储键值
        }
        else if(mode == m2_shortcut || mode == m6_change){
            cfg_data[i++] = cfg_key[keyi].key;//存储键值
            cfg_data[i++] = cfg_key[keyi].func;//存储功能键\切换方式
        }
        else if(mode == m3_group){
            len = cfg_key[keyi].data.size();//获取长度
            cfg_data[i++] = len;//存储长度
            for(int j = 0; j < len; j++){
                cfg_data[i++] = cfg_key[keyi].data.at(j);//存储mode3数据
            }
        }
        else if(mode == m4_move || mode == m5_click){
            cfg_data[i++] = (cfg_key[keyi].x >> 8) & 0xFF;
            cfg_data[i++] = cfg_key[keyi].x & 0xFF;
            cfg_data[i++] = (cfg_key[keyi].y >> 8) & 0xFF;
            cfg_data[i++] = cfg_key[keyi].y & 0xFF;
        }
        else if(mode == m7_fast){
            cfg_data[i++] = cfg_key[keyi].key;//存储键值
            cfg_data[i++] = cfg_key[keyi].func;//存储配置
            cfg_data[i++] = (cfg_key[keyi].t >> 8) & 0xFF;//周期高8位
            cfg_data[i++] = cfg_key[keyi].t & 0xFF;//周期低8位
        }
    }
    
    if(i > 476) return false;
    
    i = 476;
    cfg_data[i++] = sbox_rgb_r->value();//R
    cfg_data[i++] = sbox_rgb_g->value();//G
    cfg_data[i++] = sbox_rgb_b->value();//B
    cfg_data[i++] = cbox_key_turn->currentIndex();//键盘方向(479)
    cfg_data[i++] = (sbox_w->value() >> 8) & 0xFF;//屏幕宽
    cfg_data[i++] = sbox_w->value() & 0xFF;
    cfg_data[i++] = (sbox_h->value() >> 8) & 0xFF;//屏幕高
    cfg_data[i++] = sbox_h->value() & 0xFF;
    
    cfg_data[i++] = cbox_r[0]->currentIndex() | (cbox_r_key[0]->currentIndex() << 4);//摇杆1模式(484)
    cfg_data[i++] = rk_key[0];//摇杆1按键
    cfg_data[i++] = rk_key[1];//摇杆1上按键
    cfg_data[i++] = rk_key[2];//摇杆1下按键
    cfg_data[i++] = rk_key[3];//摇杆1左按键
    cfg_data[i++] = rk_key[4];//摇杆1右按键
    cfg_data[i++] = 0;//func
    uint8_t dir_ec = 0;
    dir_ec |= ckbox_r_r[0]->isChecked() ? 0x04 : 0x00;
    dir_ec |= ckbox_r_y[0]->isChecked() ? 0x02 : 0x00;
    dir_ec |= ckbox_r_x[0]->isChecked() ? 0x01 : 0x00;
    cfg_data[i++] = dir_ec;//摇杆1方向
    cfg_data[i++] = sbox_r[0]->value();//摇杆1死区
    cfg_data[i++] = sbox_v[0]->value();//摇杆1速度
    cfg_data[i++] = 0;//rsv1
    cfg_data[i++] = 0;//rsv2
    
    cfg_data[i++] = cbox_e[0]->currentIndex() | (cbox_e_key[0]->currentIndex() << 4);//旋钮1模式(496)
    cfg_data[i++] = ec_key[0];//旋钮1按键
    cfg_data[i++] = ec_key[1];//旋钮1逆时针按键
    cfg_data[i++] = ec_key[2];//旋钮1顺时针按键
    cfg_data[i++] = 0;//func
    cfg_data[i++] = ckbox_e[0]->isChecked() ? 0x01 : 0x00;//方向
    cfg_data[i++] = cbox_e[1]->currentIndex() | (cbox_e_key[1]->currentIndex() << 4);//旋钮2模式(502)
    cfg_data[i++] = ec_key[3];//旋钮2按键
    cfg_data[i++] = ec_key[4];//旋钮2逆时针按键
    cfg_data[i++] = ec_key[5];//旋钮2顺时针按键
    cfg_data[i++] = 0;//func
    cfg_data[i++] = ckbox_e[1]->isChecked() ? 0x01 : 0x00;//方向
    
    uint8_t rgb_cycle = cbox_wave->currentIndex() | (cbox_colorful->currentIndex() << 4);//灯周期
    uint8_t rgb_mode = 0;
    rgb_mode |= ckbox_sys_rgb_rk->isChecked() ? 0x80 : 0x00;//RGB摇杆映射
    rgb_mode |= ckbox_sys_rgb_clicker->isChecked() ? 0x40 : 0x00;//RGB自动连点指示
    rgb_mode |= ckbox_sys_rgb_m3->isChecked() ? 0x20 : 0x00;//RGB按键组指示
    rgb_mode |= cbox_rgb_delay->currentIndex() & 0x0F;//配置指示时间
    
    cfg_data[i++] = rgb_cycle;//灯周期
    cfg_data[i++] = rgb_mode;//灯设置
    cfg_data[i++] = sbox_rgb_light->value();//RGB亮度
    //cfg_data[i++] = sbox_light->value() | (sbox_light_rgb->value() << 4);//灯亮度
    cfg_data[i] = cbox_pri->currentIndex() ? 0x00 : 0x01;//总优先级
    
    return true;
}

bool config::read_cfg_data()//读出配置数组
{
    uint16_t i = 0;//访问cfg_data
    
    for(uint8_t keyi = 0; keyi < KEY_NUM; keyi++){//加载16个按键的配置
//        label_k[keyi]->setText("[]");//显示清空
        clear_key(keyi);//清除原有模式
        if(cfg_data[i++] != keyi + 1) return false;//若id不对则退出
        cfg_key[keyi].mode = cfg_data[i++];//载入模式
        if(cfg_key[keyi].mode <= 13){//模式在范围内
            set_key_mode(keyi,cfg_key[keyi].mode);//设置显示模式
        }
        else{
            cfg_key[keyi].mode = 0;//都不对则清除模式
            return false;
        }
        if(cfg_key[keyi].mode == m1_button){
            set_mode1_key(keyi,cfg_data[i++]);
        }
        else if(cfg_key[keyi].mode == m2_shortcut){
            set_mode2_key(keyi,cfg_data[i],cfg_data[i + 1]);
            i += 2;
        }
        else if(cfg_key[keyi].mode == m3_group){
//            cfg_key[keyi].data.clear();
//            cfg_key[keyi].strNum.clear();
//            cfg_key[keyi].str.clear();
            
            uint8_t len = cfg_data[i++];//暂存长度
            uint16_t end = i + len;
            while(i < end){
                if(cfg_data[i] == kv_shortcut){//kv_shortcut,key,func
                    add_mode3_func(keyi,cfg_data[i+1],cfg_data[i+2]);
                    i += 3;
                }
                else if(cfg_data[i] == kv_point){//kv_point,x_H,x_L,y_H,y_L
                    uint16_t x = (cfg_data[i + 1] << 8) | cfg_data[i + 2];
                    uint16_t y = (cfg_data[i + 3] << 8) | cfg_data[i + 4];
                    add_mode3_pos(keyi,x,y);
                    i += 5;
                }
                else if(cfg_data[i] == kv_delay){//kv_delay,t_H,t_L
                    add_mode3_delay(keyi,(cfg_data[i + 1] << 8) | cfg_data[i + 2]);
                    i += 3;
                }
                else if(cfg_data[i] == kv_shift){//kv_shift,key
                    add_mode3_key(keyi,cfg_data[i+1],1);
                    i += 2;
                }
                else if(cfg_data[i] == kv_loop){//kv_loop
                    add_mode3_loop(keyi,cfg_data[i + 1],(cfg_data[i + 2] << 8) | cfg_data[i + 3]);
                    i += 4;
                }
                else if(cfg_data[i] == kv_report){//kv_report
                    add_mode3_report(keyi,cfg_data[i + 1]);
                    i += 2;
                }
                else{
                    add_mode3_key(keyi,cfg_data[i],0);
                    i += 1;
                }
            }
            pte_mode3Box->clear();
        }
        else if(cfg_key[keyi].mode == m4_move || cfg_key[keyi].mode == m5_click){
            uint16_t x = (cfg_data[i + 0] << 8) | cfg_data[i + 1];
            uint16_t y = (cfg_data[i + 2] << 8) | cfg_data[i + 3];
            set_mode4_key(keyi,x,y);
            i += 4;
        }
        else if(cfg_key[keyi].mode == m6_change){
            set_mode6_key(keyi,cfg_data[i],cfg_data[i + 1]);
            i += 2;
        }
        else if(cfg_key[keyi].mode == m7_fast){
            set_mode7_key(keyi,cfg_data[i],cfg_data[i + 1]);//载入按键及配置
            uint16_t t = (cfg_data[i + 2] << 8) | cfg_data[i + 3];
            set_mode7_time(keyi,t);//载入周期
            i += 4;
        }
    }
    
    i = 476;
    sbox_rgb_r->setValue(cfg_data[i++]);//R
    sbox_rgb_g->setValue(cfg_data[i++]);//G
    sbox_rgb_b->setValue(cfg_data[i++]);//B
    cbox_key_turn->setCurrentIndex(cfg_data[i++]);//键盘方向(479)
    sbox_w->setValue((uint16_t)((cfg_data[i] << 8) | cfg_data[i + 1]));//屏幕宽
    i += 2;
    sbox_h->setValue((uint16_t)((cfg_data[i] << 8) | cfg_data[i + 1]));//屏幕高
    i += 2;
    
    cbox_r[0]->setCurrentIndex(cfg_data[i] & 0x0F);//摇杆1模式
    cbox_r_key[0]->setCurrentIndex((cfg_data[i++] >> 4) & 0x0F);//摇杆1按键模式
    set_rk_key(0,cfg_data[i++]);//摇杆1按键
    set_rk_key(1,cfg_data[i++]);//摇杆1上按键
    set_rk_key(2,cfg_data[i++]);//摇杆1下按键
    set_rk_key(3,cfg_data[i++]);//摇杆1左按键
    set_rk_key(4,cfg_data[i++]);//摇杆1右按键
    cfg_data[i++];//func
    ckbox_r_r[0]->setChecked(cfg_data[i] & 0x04);
    ckbox_r_y[0]->setChecked(cfg_data[i] & 0x02);
    ckbox_r_x[0]->setChecked(cfg_data[i++] & 0x01);//反向
    sbox_r[0]->setValue(cfg_data[i++]);//摇杆1死区
    sbox_v[0]->setValue(cfg_data[i++]);//摇杆1速度
    cfg_data[i++];//rsv1
    cfg_data[i++];//rsv2
    
    cbox_e[0]->setCurrentIndex(cfg_data[i] & 0x0F);//旋钮1模式
    cbox_e_key[0]->setCurrentIndex((cfg_data[i++] >> 4) & 0x0F);//旋钮1按键模式
    set_ec_key(0,cfg_data[i++]);//旋钮1按键
    set_ec_key(1,cfg_data[i++]);//旋钮1逆时针按键
    set_ec_key(2,cfg_data[i++]);//旋钮1顺时针按键
    cfg_data[i++];//func
    ckbox_e[0]->setChecked(cfg_data[i++] & 0x01);//反向
    cbox_e[1]->setCurrentIndex(cfg_data[i] & 0x0F);//旋钮2模式
    cbox_e_key[1]->setCurrentIndex((cfg_data[i++] >> 4) & 0x0F);//旋钮2按键模式
    set_ec_key(3,cfg_data[i++]);//旋钮2按键
    set_ec_key(4,cfg_data[i++]);//旋钮2逆时针按键
    set_ec_key(5,cfg_data[i++]);//旋钮2顺时针按键
    cfg_data[i++];//func
    ckbox_e[1]->setChecked(cfg_data[i++] & 0x01);//反向
    
    cbox_colorful->setCurrentIndex((cfg_data[i] >> 4) & 0x0F);//指示灯变色
    cbox_wave->setCurrentIndex(cfg_data[i++] & 0x0F);//指示灯呼吸
    
//    cbox_pri_r->setCurrentIndex((cfg_data[i] >> 7) & 0x01);//摇杆优先级
//    cbox_pri_e->setCurrentIndex((cfg_data[i] >> 6) & 0x01);//旋钮优先级
    ckbox_sys_rgb_rk->setChecked(cfg_data[i] & 0x80);//RGB摇杆映射
    ckbox_sys_rgb_clicker->setChecked(cfg_data[i] & 0x40);//RGB自动连点指示
    ckbox_sys_rgb_m3->setChecked(cfg_data[i] & 0x20);//RGB按键组指示
    
    cbox_rgb_delay->setCurrentIndex(cfg_data[i++] & 0x0F);//配置指示时间
    
    sbox_rgb_light->setValue(cfg_data[i++]);//RGB亮度
    
    cbox_pri->setCurrentIndex(!cfg_data[i]);//总优先级
    
    return true;
}

bool config::write_mode3_data(uint8_t keyi)
{
    memset(cfg_data_key,0,257);
    
    uint8_t len = cfg_key[keyi].data.size();//获取长度
    cfg_data_key[256] = len;//存储长度
    uint8_t i = 0;
    for(int j = 0; j < len; j++){
        cfg_data_key[i++] = cfg_key[keyi].data.at(j);//存储mode3数据
    }
    return true;
}

bool config::read_mode3_data(uint8_t keyi)
{
    cfg_key[keyi].data.clear();
    cfg_key[keyi].strNum.clear();
    cfg_key[keyi].str.clear();
    uint8_t i = 0;
    
    uint8_t len = cfg_data_key[256];//暂存长度
    uint16_t end = i + len;
    
    while(i < end){
        if(cfg_data_key[i] == kv_shortcut){//kv_shortcut,key,func
            add_mode3_func(keyi,cfg_data_key[i+1],cfg_data_key[i+2]);
            i += 3;
        }
        else if(cfg_data_key[i] == kv_point){//kv_point,x_H,x_L,y_H,y_L
            uint16_t x = (cfg_data_key[i + 1] << 8) | cfg_data_key[i + 2];
            uint16_t y = (cfg_data_key[i + 3] << 8) | cfg_data_key[i + 4];
            add_mode3_pos(keyi,x,y);
            i += 5;
        }
        else if(cfg_data_key[i] == kv_delay){//kv_delay,t_H,t_L
            add_mode3_delay(keyi,(cfg_data_key[i + 1] << 8) | cfg_data_key[i + 2]);
            i += 3;
        }
        else if(cfg_data_key[i] == kv_shift){//kv_shift,key
            add_mode3_key(keyi,cfg_data_key[i+1],1);
            i += 2;
        }
        else if(cfg_data[i] == kv_loop){//kv_loop
            add_mode3_loop(keyi,cfg_data[i + 1],(cfg_data[i + 2] << 8) | cfg_data[i + 3]);
            i += 4;
        }
        else if(cfg_data[i] == kv_report){//kv_report
            add_mode3_report(keyi,cfg_data[i + 1]);
            i += 2;
        }
        else{
            add_mode3_key(keyi,cfg_data_key[i],0);
            i += 1;
        }
    }
    return true;
}

const uint8_t mode_to_index[] = {0,1,2,3,4,5,6,7,8,9,10};

void config::set_key_mode(uint8_t keyi, uint8_t mode)//设置按键模式
{
    cbox_k[keyi]->setCurrentIndex(mode_to_index[mode]);//设置显示模式
}

uint8_t config::get_key_mode(uint8_t keyi)//获取按键模式
{
    uint8_t mode = cbox_k[keyi]->currentIndex();//获取显示模式
    for(uint8_t i = 0; i < sizeof(mode_to_index); i++){
        if(mode == mode_to_index[i]) return i;
    }
    return 0;
}





















