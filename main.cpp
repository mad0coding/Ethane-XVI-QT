#include "widget.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    const QString VersionStr = "V1.0-1009";//详细版本 后面一半仅作记录不显示
    QApplication a(argc, argv);
    Widget mywidget;
    mywidget.setWindowTitle("复合外设 Ethane-XVI 配置程序 " + VersionStr.section("-", 0, 0));//设置窗口名称
    mywidget.show();
    mywidget.setFocusPolicy(Qt::StrongFocus);
    
    setbuf(stdout, NULL);//设置printf缓冲为0,使立即输出
    
    if(argc == 2){//若从配置文件打开程序
        QString auto_fileName = QString::fromLocal8Bit(argv[1]);//获取配置文件路径
        if(auto_fileName.length() >= 2){
            QString fileConvert = "";//转换后的文件名
            for(int i = 0; i < auto_fileName.length(); i++){
                if(auto_fileName[i] == '\\' && i + 1 < auto_fileName.length()){//斜杠转换
                    if(auto_fileName[i + 1] == '\\'){
                        fileConvert += '/';
                        i++;
                    }
                    else fileConvert += auto_fileName[i];
                }
                else fileConvert += auto_fileName[i];
            }
            QFileInfo fileinfo = QFileInfo(fileConvert);//获取文件信息来比对后缀(不区分大小写)
            if(fileinfo.suffix().compare("etcfg", Qt::CaseInsensitive) == 0){//键盘配置文件后缀
                mywidget.fileNow = fileConvert;
                mywidget.openCfgFile();//打开键盘配置文件
            }
            else if(fileinfo.suffix().compare("etlight", Qt::CaseInsensitive) == 0){//灯效配置文件后缀
                mywidget.clrUnit->fileNow = fileConvert;
                mywidget.clrUnit->openLightFile();//打开灯效配置文件
                mywidget.ui->tabWidget->setCurrentIndex(4);//跳转到灯效配置页
            }
        }
    }
    
    a.exec();
    
    return 0;
}
