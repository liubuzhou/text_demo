#include "widget.h"
#include "ui_widget.h"

#pragma comment(lib,"C:/Users/90546/Desktop/Qt/abbControl/CppDll.lib")

#include<QMessageBox>
#include<QTimer>
#include<QStringList>
#include<QComboBox>
#include<QFileDialog>
#include<QFile>
#include<QIODevice>
#include<QTextCodec>

__declspec(dllimport) void api_link(char** strqt);
__declspec(dllimport) void api_mState(char** state);
__declspec(dllimport) void api_mShowLog(char** log);

__declspec(dllimport) void api_MotorOn();
__declspec(dllimport) void api_MotorOff();
__declspec(dllimport) void api_mStart();
__declspec(dllimport) void api_mStop();
__declspec(dllimport) void api_mPPtoMain();


__declspec(dllimport) void api_PPtoModule(char* mModule, int mRow);
__declspec(dllimport) void api_mPutFile(char* putPath);
__declspec(dllimport) void api_mLoadModule(char* loadPath);
__declspec(dllimport) void api_mDownLoadModule(char* moduleName, char* savePath);
__declspec(dllimport) void api_mDeleteModule(char* deleteName);

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    timer = new QTimer(this);

    ui->tableWidget->setColumnCount(7);
    ui->tableWidget->setRowCount(1);

    QString str="IP地址, 系统名称, 控制器名, version, avaliability, 是否虚拟, NetID";
    QStringList qstrList = str.split(",");

    ui->tableWidget->setHorizontalHeaderLabels(qstrList);

    ui->comboBox->addItem("MainModule");






    connect(timer,&QTimer::timeout,[=](){
        char* state = NULL;
        api_mState(&state);
        QString strState = QString(state);
        ui->labX->setText(strState.section("/",10,10));
        ui->labY->setText(strState.section("/",11,11));
        ui->labZ->setText(strState.section("/",12,12));
        ui->labQ1->setText(strState.section("/",13,13));
        ui->labQ2->setText(strState.section("/",14,14));
        ui->labQ3->setText(strState.section("/",15,15));
        ui->labQ4->setText(strState.section("/",16,16));

        ui->labJ1->setText(strState.section("/",4,4));
        ui->labJ2->setText(strState.section("/",5,5));
        ui->labJ3->setText(strState.section("/",6,6));
        ui->labJ4->setText(strState.section("/",7,7));
        ui->labJ5->setText(strState.section("/",8,8));
        ui->labJ6->setText(strState.section("/",9,9));

        ui->labMotorState->setText(strState.section("/",0,0));
//        ui->labX->setText(strState.section("/",1,1));
//        ui->labX->setText(strState.section("/",2,2));
//        ui->labX->setText(strState.section("/",3,3));

    });

}

Widget::~Widget()
{
    delete ui;
}

void Widget::showLog()
{
    char* log = NULL;
    api_mShowLog(& log);

    QString qstr=QString::fromLocal8Bit(log);

    QTextCodec *gbk = QTextCodec::codecForName("GBK");//采用GBK编码
    QString sfk=gbk->toUnicode((qstr.toLocal8Bit()));

    QStringList list = sfk.split("{");//QString字符串分割函数
    for(int i=list.size()-6;i<list.size();i++){
        ui->listWidgetLog->addItem(list[i]);
    }

}


void Widget::on_btnScan_clicked()
{
    char* scan = NULL;
    api_link(&scan);
    QString strscan = QString(scan);
    ui->textEditModel->setPlainText(strscan);
    timer->start(500);
//    showLog();
    for(int i=0; i<7;++i){
        ui->tableWidget->setItem(0,i,new QTableWidgetItem(strscan.section("/",i,i)));
    }
}

void Widget::on_btnMotorOn_clicked()
{
    api_MotorOn();
//    showLog();
}

void Widget::on_btnMotorOff_clicked()
{
    api_MotorOff();
//    showLog();
}

void Widget::on_btnStart_clicked()
{
    api_mStart();
//    showLog();
}

void Widget::on_btnStop_clicked()
{
    api_mStop();
//    showLog();
}



void Widget::on_btnPPtoMain_clicked()
{
    api_mPPtoMain();
//    showLog();
}

void Widget::on_btnPPMove_clicked()
{
    QByteArray str = ui->comboBox->currentText().toUtf8();
    char * ptm=str.data();
    int a=ui->spinBox->text().toInt();
    api_PPtoModule(ptm,a);
//    showLog();

}

void Widget::on_btnPut_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("open"), "../");
    QByteArray bytearray=filename.toUtf8();
    char * charx = bytearray.data();
    api_mPutFile(charx);
//    showLog();
}

void Widget::on_btnDelete_clicked()
{
    QByteArray ba=ui->lineEditDeleteMod->text().toUtf8();
    char * ar=ba.data();
    api_mDeleteModule(ar);
//    showLog();
}

void Widget::on_btnDownLoad_clicked()
{
    //需要下载的Mod
    QByteArray ba=ui->lineEditDownloadMod->text().toUtf8();
    char * ar=ba.data();

    //下载储存的路径
    QString filename = "/hd0a/2400-102056/HOME/";
    QByteArray savefile=filename.toUtf8();
    char * arr=savefile.data();

    api_mDownLoadModule(ar,arr);
//    showLog();
}

void Widget::on_btnLoad_clicked()
{
    QString filename = "/hd0a/2400-102056/HOME/" + ui->lineEditMod->text() + ".MOD";
//    QString filename = "C:/Users/90546/Documents/RobotStudio/Solutions/hCutXC/Systems/System1/HOME/" + ui->lineEditMod->text() + ".MOD";

    int nindex= filename.lastIndexOf('/'); //查找最后一个/
    QString str=filename.mid(nindex); //从最后一个/一直到字符串结尾
    str = str.left(str.size() - 4);  /* 取data左边size - 1长度的数据 */
    str.remove(0, 1);

    QByteArray bytearray=filename.toUtf8();
    char * charx = bytearray.data();
    api_mLoadModule(charx);

    ui->comboBox->addItem(str);


    QFile file(filename);
    file.open(QIODevice::ReadOnly);
    QByteArray qba = file.readAll();
    ui->textEditModel->setPlainText(qba);
    file.close();
//    showLog();



}
