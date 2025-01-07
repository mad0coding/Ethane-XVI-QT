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
    
    //测试代码↓
//    QString productString = QString::fromStdWString(hidDev->getProductString());
//    QString manufacturerString = QString::fromStdWString(hidDev->getManufacturer());
//    QString serialString = QString::fromStdWString(hidDev->getSerial());
//    qDebug()<<productString;
//    qDebug()<<manufacturerString;
//    qDebug()<<serialString;
    //测试代码↑
    
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

    if(cmd == CHID_CMD_CFG_KEY || cmd == CHID_CMD_CFG_LIGHT){//配置或灯效连接命令
        if(cmd == 0){
            memcpy(writeBuf, "DKB", 3);//填入配置连接指令
            writeBuf[3] = '1' + inBuf[0];//填入配置存储位置
        }
        else{
            memcpy(writeBuf, "DLT", 3);//填入灯效连接指令
            writeBuf[3] = '1' + inBuf[0];//填入灯效存储位置
        }
        
        ret = hid_write_read(writeBuf, readBuf);//HID先写后读
        if(ret != CHID_OK){//失败
            hid_close();//HID设备关闭
            return ret;
        }

        if(readBuf[0] == 'R' && readBuf[1] == writeBuf[1] && readBuf[2] == writeBuf[2]){//若正确响应
            return CHID_OK;
        }
    }
    else if(cmd == CHID_CMD_KEY_FLT){//修改按键滤波参数命令
        memcpy(writeBuf, "CKF", 3);//填入修改命令
        writeBuf[3] = inBuf[0];//填入按键滤波参数
        
        ret = hid_write_read(writeBuf, readBuf);//HID先写后读
        hid_close();//HID设备关闭
        if(ret != CHID_OK) return ret;//失败
        
        if(readBuf[0] == writeBuf[1] && readBuf[1] == writeBuf[2]){//若正确响应
            if(outBuf){
                memcpy(outBuf, &readBuf[2], 2);
            }
            return CHID_OK;
        }
    }
    else if(cmd == CHID_CMD_RK_ADC){//摇杆校正命令
        memcpy(writeBuf, "CRK", 3);//填入校正命令
        
        ret = hid_write_read(writeBuf, readBuf);//HID先写后读
        hid_close();//HID设备关闭
        if(ret != CHID_OK) return ret;//失败
        
        if(readBuf[0] == writeBuf[1] && readBuf[1] == writeBuf[2]){//若正确响应
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
    else if(cmd == CHID_CMD_EC_FREQ){//修改旋钮倍频参数命令
        memcpy(writeBuf, "CEC", 3);//填入修改命令
        writeBuf[3] = inBuf[0];//填入旋钮倍频参数
        writeBuf[4] = inBuf[1];
        
        ret = hid_write_read(writeBuf, readBuf);//HID先写后读
        hid_close();//HID设备关闭
        if(ret != CHID_OK) return ret;//失败
        
        if(readBuf[0] == writeBuf[1] && readBuf[1] == writeBuf[2]){//若正确响应
            if(outBuf){
                memcpy(outBuf, &readBuf[2], 4);
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
    else if(cmd == CHID_CMD_PARA){//参数读取命令
        memcpy(writeBuf, "BGPM", 4);//填入命令
        
        ret = hid_write_read(writeBuf, readBuf);//HID先写后读
        hid_close();//HID设备关闭
        if(ret != CHID_OK) return ret;//失败
        
        if(readBuf[0] == 'R' && readBuf[1] == writeBuf[1]
           && readBuf[2] == writeBuf[2] && readBuf[3] == writeBuf[3]){//若正确响应
            *(uint16_t*)(outBuf + 0) = endianConvert16(*(uint16_t*)(readBuf + 4));
            *(uint16_t*)(outBuf + 2) = endianConvert16(*(uint16_t*)(readBuf + 6));
            *(uint16_t*)(outBuf + 4) = readBuf[8];
            *(uint16_t*)(outBuf + 6) = (readBuf[9] & 0x02) * 5 + (readBuf[9] & 0x01);
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















