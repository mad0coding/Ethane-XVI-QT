#include "HidCom.h"

#include "hidapi.h"
#include "tools.h"


HidApi *hidApi = NULL;//HID
HidDevice *hidDev = NULL;//HID设备

uint16_t findVid = 0;
uint16_t findPid = 0;
uint16_t findUsagePage = 0xFFFF;


QString CHID_to_str(uint8_t ret)//HID返回值转字符串
{
    switch(ret){
    case CHID_OK:return "HID OK";
    case CHID_NO_DEV:return "No Device Found";
    case CHID_MULTI_DEV:return "More Than One Device";
    case CHID_ERR_INIT:return "HID Init Failed";
    case CHID_ERR_OPEN:return "HID Open Failed";
    case CHID_ERR_CLOSE:return "HID Close Failed";
    case CHID_ERR_WRITE:return "HID Write Failed";
    case CHID_ERR_READ:return "HID Read Failed";
    case CHID_BAD_REP:return "No Correct Response";
    case CHID_INV_CMD:return "Invalid Command";
    default:return "HID Unknown State";
    }
}

uint8_t hid_init(void)//HID初始化
{
    hidApi = new HidApi();
    hidDev = new HidDevice();//HID设备
    if(!hidApi || !hidDev) return CHID_ERR_INIT;
    return CHID_OK;
}

uint8_t hid_set_para(uint16_t fdVid, uint16_t fdPid, uint16_t fdUsagePage)//HID查找参数设置
{
    findVid = fdVid;
    findPid = fdPid;
    findUsagePage = fdUsagePage;
    return CHID_OK;
}

uint8_t hid_find_open(void)//HID设备查找并打开
{
    HidDeviceList devList, matchList;//HID设备列表 匹配设备列表
    if(!hidApi->isInitialized()) return CHID_ERR_INIT;//HidApi未成功初始化则退出
    devList = hidApi->scanDevices(findVid, findPid);//按VID和PID扫描设备
    int findDevNum = 0;//设备计数
    for(size_t i = 0; i < devList.size(); i++){//在VID和PID符合的设备中进一步查找
        if(devList[i].getUsagePage() == findUsagePage){//匹配UsagePage 实际主要是为了匹配端点
            findDevNum++;//设备计数
            *hidDev = devList[i];//选定该设备
            matchList.push_back(devList[i]);//加入匹配列表
        }
    }
    if(findDevNum == 0) return CHID_NO_DEV;//未找到设备则退出
    if(findDevNum > 1){//若有多个设备
        QStringList items;//序列号字符串列表
        for(int i = 0; i < matchList.size(); i++){
            //QString productStr = QString::fromStdWString(hidDev->getProductString());
            //QString manufacturerStr = QString::fromStdWString(hidDev->getManufacturer());
            QString serialStr = QString::fromStdWString(matchList[i].getSerial());//获取序列号
            items.append(serialStr);//填入序列号
        }
        bool ok;
        QString item = QInputDialog::getItem(NULL, "多设备", "发现多个设备 选择序列号:", items, 0, false, &ok);

        if(!ok) return CHID_MULTI_DEV;//退出
        else{//选择了一个
            for(int i = 0; i < matchList.size(); i++){//查找选中了哪个
                if(item == items[i]) *hidDev = matchList[i];//选定该设备
            }
        }
    }
    //if(!hidDev->isInitialized()) return CHID_ERR_INIT;//HID设备未初始化则退出
    if(!hidDev->open()) return CHID_ERR_OPEN;//HID设备打开失败则退出
    
    return CHID_OK;
}

uint8_t hid_close()//HID设备关闭
{
    return (hidDev->close()) ? CHID_OK : CHID_ERR_CLOSE;//关闭HID设备
}

uint8_t hid_write_read(uint8_t *writeBuf, uint8_t *readBuf)//HID先写后读
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

uint8_t hid_send_cmd(uint8_t cmd, uint8_t *inBuf, uint8_t *outBuf)//HID向设备写入命令
{
    uint8_t ret = CHID_OK;
    ret = hid_find_open();//HID设备查找并打开
    if(ret != CHID_OK){//失败
        return ret;
    }
    
    uint8_t writeBuf[64], readBuf[64];//读写缓存
    memset(writeBuf, 0, sizeof(writeBuf));
    memset(readBuf, 0, sizeof(readBuf));

    if(cmd == CHID_CMD_CFG_KEY || cmd == CHID_CMD_CFG_LIGHT){//键盘配置或灯效配置连接命令
        if(cmd == 0){
            memcpy(writeBuf, "DKBC", 4);//填入配置连接指令
            writeBuf[4] = '1' + inBuf[0];//填入配置存储位置
        }
        else{
            memcpy(writeBuf, "DLTC", 4);//填入灯效连接指令
            writeBuf[4] = '1' + inBuf[0];//填入灯效存储位置
        }
        
        ret = hid_write_read(writeBuf, readBuf);//HID先写后读
        if(ret != CHID_OK){//失败
            hid_close();//HID设备关闭
            return ret;
        }

        if(readBuf[0] == 'R' && readBuf[1] == writeBuf[1]
           && readBuf[2] == writeBuf[2] && readBuf[3] == writeBuf[3]){//若正确响应
            return CHID_OK;
        }
    }
    else if(cmd == CHID_CMD_KEY_FLT){//修改按键消抖参数命令
        memcpy(writeBuf, "CKYT", 4);//填入修改命令
        writeBuf[4] = inBuf[0];//填入按键消抖参数
        
        ret = hid_write_read(writeBuf, readBuf);//HID先写后读
        hid_close();//HID设备关闭
        if(ret != CHID_OK) return ret;//失败
        
        if(readBuf[0] == 'R' && readBuf[1] == writeBuf[1]
           && readBuf[2] == writeBuf[2] && readBuf[3] == writeBuf[3]){//若正确响应
            if(outBuf){
                memcpy(outBuf, &readBuf[4], 2);
            }
            return CHID_OK;
        }
    }
    else if(cmd == CHID_CMD_RK_MID){//摇杆中位校正命令
        memcpy(writeBuf, "CRKM", 4);//填入校正命令
        
        ret = hid_write_read(writeBuf, readBuf);//HID先写后读
        hid_close();//HID设备关闭
        if(ret != CHID_OK) return ret;//失败
        
        if(readBuf[0] == 'R' && readBuf[1] == writeBuf[1]
           && readBuf[2] == writeBuf[2] && readBuf[3] == writeBuf[3]){//若正确响应
            uint16_t adcValue[4];
            for(int i = 0; i < 4; i++) adcValue[i] = endianConvert16(*(uint16_t*)&readBuf[4 + 2*i]);
            if(adcValue[2] > 4095 || adcValue[3] > 4095) return CHID_BAD_REP;//数据错误
            if(outBuf){
                memcpy(outBuf, adcValue, sizeof(adcValue));
            }
            return CHID_OK;
        }
    }
    else if(cmd == CHID_CMD_RK_ZONE){//摇杆范围设置命令
        memcpy(writeBuf, "CRKZ", 4);//填入校正命令
        
        for(int i = 0; i < 4; i++) ((uint16_t*)(writeBuf + 4))[i] = endianConvert16(((uint16_t*)inBuf)[i]);
        
        ret = hid_write_read(writeBuf, readBuf);//HID先写后读
        hid_close();//HID设备关闭
        if(ret != CHID_OK) return ret;//失败
        
        if(readBuf[0] == 'R' && readBuf[1] == writeBuf[1]
           && readBuf[2] == writeBuf[2] && readBuf[3] == writeBuf[3]){//若正确响应
            uint16_t zoneValue[8];
            for(int i = 0; i < 8; i++) zoneValue[i] = endianConvert16(((uint16_t*)(readBuf + 4))[i]);
            memcpy(outBuf, zoneValue, sizeof(zoneValue));
            return CHID_OK;
        }
    }
    else if(cmd == CHID_CMD_EC_FREQ){//修改旋钮倍频参数命令
        memcpy(writeBuf, "CECD", 4);//填入修改命令
        writeBuf[4] = inBuf[0];//填入旋钮倍频参数
        writeBuf[5] = inBuf[1];
        
        ret = hid_write_read(writeBuf, readBuf);//HID先写后读
        hid_close();//HID设备关闭
        if(ret != CHID_OK) return ret;//失败
        
        if(readBuf[0] == 'R' && readBuf[1] == writeBuf[1]
           && readBuf[2] == writeBuf[2] && readBuf[3] == writeBuf[3]){//若正确响应
            if(outBuf){
                memcpy(outBuf, &readBuf[4], 4);
            }
            return CHID_OK;
        }
    }
    else if(cmd == CHID_CMD_RST){//软复位命令
        memcpy(writeBuf, "BRST", 4);//填入命令
        
        ret = hid_write_read(writeBuf, readBuf);//HID先写后读
        hid_close();//HID设备关闭
        if(ret != CHID_OK) return ret;//失败
        
        if(readBuf[0] == 'R' && readBuf[1] == writeBuf[1]
           && readBuf[2] == writeBuf[2] && readBuf[3] == writeBuf[3]){//若正确响应
            return CHID_OK;
        }
    }
    else if(cmd == CHID_CMD_BOOT){//Boot预跳转命令
        memcpy(writeBuf, "BBOT", 4);//填入命令
        
        ret = hid_write_read(writeBuf, readBuf);//HID先写后读
        hid_close();//HID设备关闭
        if(ret != CHID_OK) return ret;//失败
        
        if(readBuf[0] == 'R' && readBuf[1] == writeBuf[1]
           && readBuf[2] == writeBuf[2] && readBuf[3] == writeBuf[3]){//若正确响应
            return CHID_OK;
        }
    }
    else if(cmd == CHID_CMD_CSC){//配置切换命令
        memcpy(writeBuf, "BCSC", 4);//填入命令
        writeBuf[4] = inBuf[0] - 1 + '1';//填入切换目标
        
        ret = hid_write_read(writeBuf, readBuf);//HID先写后读
        hid_close();//HID设备关闭
        if(ret != CHID_OK) return ret;//失败
        
        if(readBuf[0] == 'R' && readBuf[1] == writeBuf[1]
           && readBuf[2] == writeBuf[2] && readBuf[3] == writeBuf[3]){//若正确响应
            outBuf[0] = readBuf[4] + 1 - '1';//旧选择
            outBuf[1] = readBuf[5] + 1 - '1';//新选择
            return CHID_OK;
        }
    }
    else if(cmd == CHID_CMD_BUZZ){//蜂鸣器模式命令
        memcpy(writeBuf, "BBUZ", 4);//填入命令
        
        ret = hid_write_read(writeBuf, readBuf);//HID先写后读
        hid_close();//HID设备关闭
        if(ret != CHID_OK) return ret;//失败
        
        if(readBuf[0] == 'R' && readBuf[1] == writeBuf[1]
           && readBuf[2] == writeBuf[2] && readBuf[3] == writeBuf[3]){//若正确响应
            return CHID_OK;
        }
    }
    else if(cmd == CHID_CMD_FW_VER){//固件版本读取命令
        memcpy(writeBuf, "BFWV", 4);//填入命令
        
        ret = hid_write_read(writeBuf, readBuf);//HID先写后读
        hid_close();//HID设备关闭
        if(ret != CHID_OK) return ret;//失败
        
        if(readBuf[0] == 'R' && readBuf[1] == writeBuf[1]
           && readBuf[2] == writeBuf[2] && readBuf[3] == writeBuf[3]){//若正确响应
            memcpy(outBuf, readBuf + 4, 4);//传出固件版本号
            return CHID_OK;
        }
    }
    else if(cmd == CHID_CMD_UUID){//序列号读取命令
        memcpy(writeBuf, "BUID", 4);//填入命令
        
        ret = hid_write_read(writeBuf, readBuf);//HID先写后读
        hid_close();//HID设备关闭
        if(ret != CHID_OK) return ret;//失败
        
        if(readBuf[0] == 'R' && readBuf[1] == writeBuf[1]
           && readBuf[2] == writeBuf[2] && readBuf[3] == writeBuf[3]){//若正确响应
            for(int i = 0; i < 3; i++) ((uint16_t*)outBuf)[i] = endianConvert16(*(uint16_t*)(readBuf + 4 + 2 * i));
            return CHID_OK;
        }
    }
    else if(cmd == CHID_CMD_INPUT){//输入读取命令
        memcpy(writeBuf, "BIPT", 4);//填入命令
        
        ret = hid_write_read(writeBuf, readBuf);//HID先写后读
        hid_close();//HID设备关闭
        if(ret != CHID_OK) return ret;//失败
        
        if(readBuf[0] == 'R' && readBuf[1] == writeBuf[1]
           && readBuf[2] == writeBuf[2] && readBuf[3] == writeBuf[3]){//若正确响应
            *(uint16_t*)(outBuf + 0) = endianConvert16(*(uint16_t*)(readBuf + 4));
            *(uint16_t*)(outBuf + 2) = endianConvert16(*(uint16_t*)(readBuf + 6));
            *(uint16_t*)(outBuf + 4) = endianConvert16(*(uint16_t*)(readBuf + 10));
            *(uint16_t*)(outBuf + 6) = endianConvert16(*(uint16_t*)(readBuf + 8));
            return CHID_OK;
        }
    }
    else if(cmd == CHID_CMD_PARA){//参数读取命令
        memcpy(writeBuf, "BGPM", 4);//填入命令
        
        ret = hid_write_read(writeBuf, readBuf);//HID先写后读
        hid_close();//HID设备关闭
        if(ret != CHID_OK) return ret;//失败
        
        if(readBuf[0] == 'R' && readBuf[1] == writeBuf[1]
           && readBuf[2] == writeBuf[2] && readBuf[3] == writeBuf[3]){//若正确响应
            *(uint16_t*)(outBuf + 0) = endianConvert16(*(uint16_t*)(readBuf + 4));//Min0
            *(uint16_t*)(outBuf + 2) = endianConvert16(*(uint16_t*)(readBuf + 8));//Max0
            *(uint16_t*)(outBuf + 4) = endianConvert16(*(uint16_t*)(readBuf + 6));//Min1
            *(uint16_t*)(outBuf + 6) = endianConvert16(*(uint16_t*)(readBuf + 10));//Max1
            *(uint16_t*)(outBuf + 8) = endianConvert16(*(uint16_t*)(readBuf + 12));//Mid0
            *(uint16_t*)(outBuf + 10) = endianConvert16(*(uint16_t*)(readBuf + 14));//Mid1
            *(uint16_t*)(outBuf + 12) = readBuf[20];//按键消抖参数
            *(uint16_t*)(outBuf + 14) = (readBuf[24] & 0x02) * 5 + (readBuf[24] & 0x01);//旋钮倍频参数
            return CHID_OK;
        }
    }
    else if(cmd == CHID_CMD_DIAG){//诊断信息读取命令
        memcpy(writeBuf, "BDGC", 4);//填入命令
        
        ret = hid_write_read(writeBuf, readBuf);//HID先写后读
        hid_close();//HID设备关闭
        if(ret != CHID_OK) return ret;//失败
        
        if(readBuf[0] == 'R' && readBuf[1] == writeBuf[1]
           && readBuf[2] == writeBuf[2] && readBuf[3] == writeBuf[3]){//若正确响应
            for(int i = 0; i < 28; i++) ((uint16_t*)outBuf)[i] = endianConvert16(*(uint16_t*)(readBuf + 4 + 2 * i));
            return CHID_OK;
        }
    }
    hid_close();//HID设备关闭
    
    if(readBuf[0] == 'R' && readBuf[1] == 'I' && readBuf[2] == 'N' && readBuf[3] == 'V'){//设备认为不是合法命令
        return CHID_INV_CMD;//非法命令
    }
    return CHID_BAD_REP;//设备无响应或错误响应
}

uint8_t hid_send_data(uint8_t cmd, uint8_t *inBuf, uint8_t *buf)//HID向设备写入数据
{
    uint8_t ret = CHID_OK;
    uint8_t writeBuf[64], readBuf[64];//读写缓存
    
    int success = 0, packNum = (cmd == CHID_CMD_CFG_KEY) ? 8 : 4;//包数
    ret = hid_send_cmd(cmd, inBuf, NULL);//写入连接命令
    if(ret == CHID_OK){//若连接成功
        for(int i = 0; i < packNum; i++){//64*8=512字节
            mySleep(5);//强制延时5ms防止HID发送卡死
            
            memcpy(writeBuf, (buf + i * 64), 64);//拷贝到写入缓存
            
            ret = hid_write_read(writeBuf, readBuf);//HID先写后读
            if(ret != CHID_OK) break;//失败
            
            if(readBuf[0] == 'R' && readBuf[1] == 'D' && readBuf[2] == i){//若正确响应
                success++;
            }
        }
    }
    hid_close();//HID设备关闭
    
    if(success == packNum) return CHID_OK;//成功个数等于包个数
    
    return ret;//失败
}















