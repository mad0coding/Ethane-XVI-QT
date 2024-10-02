#include "tools.h"

bool read_asciiArray_file(QString path, uint8_t *buf, uint16_t len)//读明文数组文件
{
    if(!path.isEmpty()){//若路径存在
        QFile file(path);
        if(file.open(QIODevice::ReadOnly)){//若打开成功
            QByteArray array = file.readAll();
            file.close();
            char* data = array.data();
            int j = 0, num = 0;
            QString strNum = "";
            for(int i = 0; i < array.length() + 1; i++){
                if(i < array.length() && data[i] >= '0' && data[i] <= '9') strNum.append(data[i]);//未越界且为数字
                else if(strNum.length() > 0){//不为数字或到末尾且字符串不空
                    num = strNum.toInt();
                    strNum = "";
                    num = LIMIT(num,0,255);
                    buf[j++] = num;
                }
                if(j >= len) return true;
            }
        }
        file.close();
    }
    return false;
}

bool read_cfg_file(QString path, uint8_t *buf, uint16_t len)//读配置文件
{
    if(!path.isEmpty()){//若路径存在
        QFile file(path);
        if(file.open(QIODevice::ReadOnly)){//若打开成功
            QByteArray array = file.readAll();
            if(array.length() == len){//若长度为len
                char* data = array.data();
                //for(int i = 0; i < len; i++) buf[i] = data[i];//读入数据
                memcpy(buf,data,len);//读入数据
                file.close();
                return true;
            }
        }
        file.close();
    }
    return false;
}

bool write_cfg_file(QString path, uint8_t *buf, uint16_t len)//写配置文件
{
    if(!path.isEmpty()){//若路径存在
        QFile file(path);
        if(file.open(QIODevice::WriteOnly|QIODevice::Truncate)){//若打开成功
            QByteArray array = "";
            for(int i = 0; i < len; i++) array.append(buf[i]);//写入数据
            file.write(array);
            file.close();
            return true;
        }
        file.close();
    }
    return false;
}

void mySleep(int ms)//毫秒延时
{
    QTime dieTime = QTime::currentTime().addMSecs(ms);
    while( QTime::currentTime() < dieTime){
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}


uint8_t key_to_USB(int key, int Vkey)//QT键值转USB键值
{
    if(Vkey >= 65 && Vkey <= 90) return Vkey - 61;//A~Z(4~29)
    
    if(Vkey >= 49 && Vkey <= 57) return Vkey - 19;//1~9(30~38)
    if(Vkey == 48) return 39;//0
    
    if(key == Qt::Key_Return) return 40;//Return
    if(key == Qt::Key_Escape) return 41;//Escape
    if(key == Qt::Key_Backspace) return 42;//Backspace
    if(key == Qt::Key_Tab) return 43;//Tab
    if(key == Qt::Key_Space) return 44;//Space
    if(Vkey == 189) return 45;//-_
    if(Vkey == 187) return 46;//=+
    if(Vkey == 219) return 47;//[{
    if(Vkey == 221) return 48;//]}
    if(Vkey == 220) return 49;//(|\)
    if(Vkey == 192) return 50;//`~
    if(Vkey == 186) return 51;//;:
    if(Vkey == 222) return 52;//'"
    //if(key == Qt::) return 53;
    if(Vkey == 188) return 54;//,<
    if(Vkey == 190) return 55;//.>
    if(Vkey == 191) return 56;//(/?)
    if(key == Qt::Key_CapsLock) return 57;//caps lock
    
    if(key >= Qt::Key_F1 && key <= Qt::Key_F12) return key - Qt::Key_F1 + 58;//F1~F12(58~69)
    
    if(key == Qt::Key_Print) return 70;//截图
    if(key == Qt::Key_ScrollLock) return 71;//滚动锁定
    if(key == Qt::Key_Pause) return 72;//暂停键
    if(key == Qt::Key_Insert) return 73;//Insert
    if(key == Qt::Key_Home) return 74;//Home
    if(key == Qt::Key_PageUp) return 75;//上页
    if(key == Qt::Key_Delete) return 76;//Delete
    if(key == Qt::Key_End) return 77;//End
    if(key == Qt::Key_PageDown) return 78;//下页
    if(key == Qt::Key_Right) return 79;//→
    if(key == Qt::Key_Left) return 80;//←
    if(key == Qt::Key_Down) return 81;//↓
    if(key == Qt::Key_Up) return 82;//↑
    if(key == Qt::Key_NumLock) return 83;//NumLock
    if(Vkey == 111) return 84;//(/)
    if(Vkey == 106) return 85;//(*)
    if(Vkey == 109) return 86;//-
    if(Vkey == 107) return 87;//+
    if(key == Qt::Key_Enter) return 88;//Enter
    
    if(Vkey >= 97 && Vkey <= 105) return Vkey - 8;//NUM1~NUM9(89~97)
    if(Vkey == 96) return 98;//NUM0
    
    if(Vkey == 110) return 99;//.
    
    if(key == Qt::Key_Control) return 249;//ctrl
    if(key == Qt::Key_Shift) return 250;//shift
    if(key == Qt::Key_Alt) return 251;//alt
    if(key == 16777250) return 252;//win
    
    return 0;
}

QString USB_to_str(uint8_t key, bool shift)//USB键值转按键名
{
    switch (key) {
        case 0:return "[None]";
        case 4:return "[A]";
        case 5:return "[B]";
        case 6:return "[C]";
        case 7:return "[D]";
        case 8:return "[E]";
        case 9:return "[F]";
        case 10:return "[G]";
        case 11:return "[H]";
        case 12:return "[I]";
        case 13:return "[J]";
        case 14:return "[K]";
        case 15:return "[L]";
        case 16:return "[M]";
        case 17:return "[N]";
        case 18:return "[O]";
        case 19:return "[P]";
        case 20:return "[Q]";
        case 21:return "[R]";
        case 22:return "[S]";
        case 23:return "[T]";
        case 24:return "[U]";
        case 25:return "[V]";
        case 26:return "[W]";
        case 27:return "[X]";
        case 28:return "[Y]";
        case 29:return "[Z]";
        case 30:return shift ? "[!]" : "[1]";
        case 31:return shift ? "[@]" : "[2]";
        case 32:return shift ? "[#]" : "[3]";
        case 33:return shift ? "[$]" : "[4]";
        case 34:return shift ? "[%]" : "[5]";
        case 35:return shift ? "[^]" : "[6]";
        case 36:return shift ? "[&]" : "[7]";
        case 37:return shift ? "[*]" : "[8]";
        case 38:return shift ? "[(]" : "[9]";
        case 39:return shift ? "[)]" : "[0]";
        case 40:return "[Return]";
        case 41:return "[Escape]";
        case 42:return "[Backspace]";
        case 43:return "[Tab]";
        case 44:return "[Space]";
        case 45:return shift ? "[_]" : "[-]";
        case 46:return shift ? "[+]" : "[=]";
        case 47:return shift ? "[{]" : "[ [ ]";
        case 48:return shift ? "[}]" : "[ ] ]";
        case 49:return shift ? "[|]" : "[\\]";
        case 50:return shift ? "[~]" : "[`]";
        case 51:return shift ? "[:]" : "[;]";
        case 52:return shift ? "[\"]" : "[']";
        case 53:return shift ? "[Tilde]" : "[Grave Accent]";
        case 54:return shift ? "[<]" : "[,]";
        case 55:return shift ? "[>]" : "[.]";
        case 56:return shift ? "[?]" : "[/]";
        case 57:return "[CapsLock]";
        case 58:return "[F1]";
        case 59:return "[F2]";
        case 60:return "[F3]";
        case 61:return "[F4]";
        case 62:return "[F5]";
        case 63:return "[F6]";
        case 64:return "[F7]";
        case 65:return "[F8]";
        case 66:return "[F9]";
        case 67:return "[F10]";
        case 68:return "[F11]";
        case 69:return "[F12]";
        case 70:return "[PrintScreen]";
        case 71:return "[ScrollLock]";
        case 72:return "[Pause]";
        case 73:return "[Insert]";
        case 74:return "[Home]";
        case 75:return "[PageUp]";
        case 76:return "[Delete]";
        case 77:return "[End]";
        case 78:return "[PageDown]";
        case 79:return "[Right]";
        case 80:return "[Left]";
        case 81:return "[Down]";
        case 82:return "[Up]";
        case 83:return "[NumLock]";
        case 84:return "[NUM /]";
        case 85:return "[NUM *]";
        case 86:return "[NUM -]";
        case 87:return "[NUM +]";
        case 88:return "[Enter]";
        case 89:return "[NUM 1]";
        case 90:return "[NUM 2]";
        case 91:return "[NUM 3]";
        case 92:return "[NUM 4]";
        case 93:return "[NUM 5]";
        case 94:return "[NUM 6]";
        case 95:return "[NUM 7]";
        case 96:return "[NUM 8]";
        case 97:return "[NUM 9]";
        case 98:return "[NUM 0]";
        case 99:return "[NUM .]";
        case 100:return "[Non-US\and|]";
        case 101:return "[Application]";
        case 102:return "[Power]";
        case 103:return "[NUM =]";
        case kv_wheel_up:return "[滚轮向上]";
        case kv_wheel_down:return "[滚轮向下]";
        case kv_vol_up:return "[音量加]";
        case kv_vol_down:return "[音量减]";
        case kv_vol_mute:return "[静音]";
        case kv_vol_stop:return "[播放/暂停]";
        case kv_vol_next:return "[下一个]";
        case kv_vol_prev:return "[上一个]";
        case kv_mouse_l:return "[鼠标左键]";
        case kv_mouse_m:return "[鼠标中键]";
        case kv_mouse_r:return "[鼠标右键]";
        case kv_ctrl:return "[Ctrl]";
        case kv_shift:return "[Shift]";
        case kv_alt:return "[Alt]";
        case kv_win:return "[Win]";
        default:return "[]";
    }
}

//H:0~colorAngle*6,S:0~255(用delta代替),V:0~255
void rgbToHsv(uint8_t vR, uint8_t vG, uint8_t vB, uint16_t* pH, uint16_t* pS, uint16_t* pV){
    uint8_t max = MAX(MAX(vR,vG),vB), min = MIN(MIN(vR,vG),vB);
    uint8_t delta = max - min;
    if(delta == 0) *pH = 0;
    else if(max == vR) *pH = colorAngle*(vG-vB)/delta;
    else if(max == vG) *pH = colorAngle*(vB-vR)/delta + colorAngle*2;
    else if(max == vB) *pH = colorAngle*(vR-vG)/delta + colorAngle*4;
    if(*pH > colorAngle*6) *pH += colorAngle*6;
    if(max == 0) *pS = 0;
    else *pS = delta;//100 * delta / max
    *pV = max;
}
void hsvToRgb(uint16_t vH, uint16_t vS, uint16_t vV, uint8_t* pR, uint8_t* pG, uint8_t* pB){
    uint8_t hi = (uint16_t)(vH / colorAngle) % 6;
    uint16_t f = vH - hi * colorAngle;
    uint8_t p = vV - vS;
    uint8_t q = vV - vS * f / colorAngle;
    uint8_t t = vV - vS * (colorAngle - f) / colorAngle;
    if(hi == 0)     {*pR = vV;   *pG = t;    *pB = p;}
    else if(hi == 1){*pR = q;    *pG = vV;   *pB = p;}
    else if(hi == 2){*pR = p;    *pG = vV;   *pB = t;}
    else if(hi == 3){*pR = p;    *pG = q;    *pB = vV;}
    else if(hi == 4){*pR = t;    *pG = p;    *pB = vV;}
    else if(hi == 5){*pR = vV;   *pG = p;    *pB = q;}
}

QPixmap rgbToPix(uint8_t r, uint8_t g, uint8_t b, uint16_t w, uint16_t h, uint8_t mode){
    QImage image(w, h, QImage::Format_RGB888);
    int d = 0;
    if(mode == 1){
        QColor rgb(r, g, b);
        uint16_t v = rgb.value();//亮度
        
        if(v < 150) d = 12 - (v * 12 / 150);//计算黑边宽度
        
        if(v && v < 6) v = v * 20 + 100;//把低亮度区拉高
        else if(v) v = MIN((v - 6) * 6 / 25 + 200, 255);
        
        rgb.setHsv(rgb.hue(), rgb.saturation(), v);
        r = rgb.red(); g = rgb.green(); b = rgb.blue();
        b = MIN(b * 12 / 10, 255);//单独提升一下蓝色的亮度
    }
    for(int i = 0; i < w; i++){
        for(int j = 0; j < h; j++){
            QRgb pixel = qRgb(r, g, b);
            uint8_t dmin = MIN(MIN(i, j), MIN(w - i - 1, h - j - 1));//离边框距离
            if(dmin < d) pixel = qRgb(0, 0, 0);//若小于d则为黑色
            image.setPixel(i, j, pixel);
        }
    }
    return QPixmap::fromImage(image);
}

uint16_t endianConvert16(uint16_t num){//16位大小端转换
    return (num << 8) | (num >> 8);//交换两个字节
}






















