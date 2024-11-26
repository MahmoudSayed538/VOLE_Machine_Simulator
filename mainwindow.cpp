#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QString>
#include<QRegularExpression>
#include <QDebug>
#include<QThread>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QPushButton>
#include <QWidget>
#include <QVector>
//some helper functions
// UI interface.

Machine* myMachine = new Machine();


void MainWindow::clearMemory()
{
    for(int i = 0 ; i<256 ; i+=2)
    {
        myMachine->load("0000",i,ui);
    }
}


void MainWindow::resetCPU()
{
    for(int i = 0 ;i<16;i++)
    {
        myMachine->accessCPU()->setR("00",i,ui);
    }myMachine->accessCPU()->setIR("0000",ui);
    myMachine->accessCPU()->setPC(0,ui);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    clearMemory();
    resetCPU();
    loadCounter=0;
    userInput="0000";
}


MainWindow::~MainWindow()
{
    delete ui;
}

void Memory::setMemory(const int& address,const string& value,Ui::MainWindow* ui)
{
    mainMemory.at(address)=value; // setting the value in my memory.

    QString hexAddress = QString("%1").arg(address, 2, 16, QChar('0')); // converting the address to hex to identify it's place in the gui tabel.
    int row1=QString(hexAddress.at(0)).toInt(nullptr,16);
    int column1=QString(hexAddress.at(1)).toInt(nullptr,16);
    QTableWidgetItem *Value = new QTableWidgetItem(QString::fromStdString(value)); // setting an item for the gui.
    ui->guiMemory->setItem(row1,column1,Value); // displaying the value on my table widget.
}

void Register::setRegist(const string& value , const int& address , Ui::MainWindow* ui)
{
    regist.at(address)=value;
    QTableWidgetItem *Value = new QTableWidgetItem(QString::fromStdString(value)); // same as memory setting.
    ui->guiRegister->setItem(address,0,Value);
}


// making sure my input is valid.

bool MainWindow::isValid(const QString& input)
{
    QRegularExpression valid("[1,2,3,4,5,6,7,8,9,A,B,C,D]{1}[0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F]{3}");
    QRegularExpressionMatch match= valid.match(input);
    return match.hasMatch();
}



// loading the memory with the user's instructions.

void Machine::load(const string& input,const int& address,Ui::MainWindow* ui)
{
    m->setMemory(address,input.substr(0,2),ui);
    m->setMemory(address+1,input.substr(2,2),ui);
}

// searches the memory for an instruction and then executes it

void Machine::run(Ui::MainWindow* ui)
{
    c->fetch(m,ui);
    c->execute(m,ui);
}


// gets the instruction from the memory and updates the program counter (PC)

void CPU::fetch(Memory* m,Ui::MainWindow* ui)
{
    setPC(PC,ui); //sets the program counter for the gui.

    setIR(m->getMemory(PC)+m->getMemory(PC+1),ui); //sets the instruction register for the gui.

    if((IR[0] == 'B' || IR[0]== 'D') && cu.jump(IR,r)) // checks if jumping is needed.
    {
        QString ir = QString::fromStdString(IR);
        QString mcell = ir.mid(2,2);
        setPC(mcell.toInt(nullptr,16),ui);
        setIR(m->getMemory(PC)+m->getMemory(PC+1),ui);
    }
    else{
        PC+=2; // updates the PC to fetch the next instruction.
    }
}

// puts a value in a specific address in the register.

void CPU::setR(const string& value , const int& address , Ui::MainWindow* ui)
{
    r->setRegist(value,address,ui);
}

//updates the IR for the backend and the gui IR .

void CPU::setIR(const string& value,Ui::MainWindow* ui)
{
    IR=value;
    QString ir = QString::fromStdString(value);
    ui->IR->setText(ir);
}

// does the same as the previous function but with the PC.
void CPU::setPC(const int& value,Ui::MainWindow* ui)
{
    PC=value;
    QString pc = QString("%1").arg(value, 2, 16, QChar('0')).toUpper();
    ui->PC->setText(pc);
}

// decides which operation to execute accoding to the operation code of the instruction.

void CPU::execute(Memory* m,Ui::MainWindow* ui)
{
    char opCode = IR[0];
    if(opCode=='1' || opCode=='2' || opCode=='3'||opCode=='4')
    {
        cu.copy(IR,ui,m,r);
    }else if(opCode == '5' || opCode == '6')
    {
        cu.add(IR,ui,r);
    }else if(opCode == '7')
    {
        cu.OR(IR,ui,r);
    }else if(opCode=='8')
    {
        cu.AND(IR,ui,r);
    }else if(opCode=='9')
    {
        cu.XOR(IR,ui,r);
    }else if (opCode=='A')
    {
        cu.rotate(IR,ui,r);
    }
}



void CU::copy(const string& IR,Ui::MainWindow* ui,Memory* m,Register* r)
{
    QString ir = QString::fromStdString(IR);
    if(IR[0]=='1')
    {
        QString mcell = ir.mid(2,2);
        QString rcell = ir.mid(1,1);
        int maddress = mcell.toInt(nullptr,16);
        int raddress = rcell.toInt(nullptr,16);
        string pattern = m->getMemory(maddress);
        r->setRegist(pattern,raddress,ui);

    }else if(IR[0]=='2')
    {
        QString rcell = ir.mid(1,1);
        int raddress = rcell.toInt(nullptr,16);
        string pattern = (ir.mid(2,2)).toStdString();
        r->setRegist(pattern,raddress,ui);

    }else if(IR[0]=='3')
    {
        QString mcell = ir.mid(2,2);
        QString rcell = ir.mid(1,1);
        int maddress = mcell.toInt(nullptr,16);
        int raddress = rcell.toInt(nullptr,16);
        string pattern = r->getRegist(raddress);
        m->setMemory(maddress,pattern,ui);
    }else
    {
        QString rcell1 = ir.mid(2,1);
        QString rcell2 = ir.mid(3,1);
        int raddress1 = rcell1.toInt(nullptr,16);
        int raddress2 = rcell2.toInt(nullptr,16);
        string pattern = r->getRegist(raddress1);
        r->setRegist(pattern,raddress2,ui);
    }
}
void CU::add(const string& IR,Ui::MainWindow* ui,Register* r)
{
    QString ir = QString::fromStdString(IR);
    QString rcell1 = ir.mid(1,1);
    QString rcell2 = ir.mid(2,1);
    QString rcell3 = ir.mid(3,1);
    int raddress1 = rcell1.toInt(nullptr,16);
    int raddress2 = rcell2.toInt(nullptr,16);
    int raddress3 = rcell3.toInt(nullptr,16);
    QString hexS = QString::fromStdString(r->getRegist(raddress2));
    QString hexT = QString::fromStdString(r->getRegist(raddress3));
    int n1 = hexS.toInt(nullptr,16);
    int n2 = hexT.toInt(nullptr,16);
    if(n1>127)
    {
        n1-=256;
    }if(n2>127)
    {
        n2-=256;
    }
    uint8_t result = n1+n2;
    QString hexResult = QString("%1").arg(result, 2, 16, QChar('0')).toUpper();
    string finalResult = hexResult.toStdString();
    r->setRegist(finalResult,raddress1,ui);

}
void CU::OR(const string& IR,Ui::MainWindow* ui,Register* r)
{
    QString ir = QString::fromStdString(IR);
    QString rcell1 = ir.mid(1,1);
    int address1 = rcell1.toInt(nullptr,16);
    QString rcell2 = ir.mid(2,1);
    int address2 = rcell2.toInt(nullptr,16);
    QString rcell3 = ir.mid(3,1);
    int address3 = rcell3.toInt(nullptr,16);
    string dataF = r->getRegist(address2);
    string dataS = r->getRegist(address3);
    string result ="";

    for (int i = 0; i < dataF.length(); i++) {
        if(dataF[i] == '1' || dataS[i] == '1') result += '1';
        else result += '0';

    }
    r->setRegist(result,address1,ui);

}
void CU::AND(const string& IR,Ui::MainWindow* ui,Register* r)
{
    QString ir = QString::fromStdString(IR);
    QString rcell1 = ir.mid(1,1);
    int address1 = rcell1.toInt(nullptr,16);
    QString rcell2 = ir.mid(2,1);
    int address2 = rcell2.toInt(nullptr,16);
    QString rcell3 = ir.mid(3,1);
    int address3 = rcell3.toInt(nullptr,16);
    string dataF = r->getRegist(address2);
    string dataS = r->getRegist(address3);
    string result ="";
    for (int i = 0; i < dataF.length(); i++) {
        if(dataF[i] == '1' && dataS[i] == '1') result += '1';
        else result += '0';

    }
    r->setRegist(result,address1,ui);
}
void CU::XOR(const string& IR,Ui::MainWindow* ui,Register* r)
{
    QString ir = QString::fromStdString(IR);
    QString rcell1 = ir.mid(1,1);
    int address1 = rcell1.toInt(nullptr,16);
    QString rcell2 = ir.mid(2,1);
    int address2 = rcell2.toInt(nullptr,16);
    QString rcell3 = ir.mid(3,1);
    int address3 = rcell3.toInt(nullptr,16);
    string dataF = r->getRegist(address2);
    string dataS = r->getRegist(address3);
    string result ="";
    for (int i = 0; i < dataF.length(); i++) {
        if((dataF[i] == '1' && dataS[i] == '0') || (dataF[i] == '0' && dataS[i] == '1'))
        {
            result += '1';
        }
        else result += '0';

    }
    r->setRegist(result,address1,ui);
}


void CU::rotate(const string& IR,Ui::MainWindow*ui,Register* r)
{
    QString ir = QString::fromStdString(IR);
    QString rcell = ir.mid(1,1);
    int raddress = rcell.toInt(nullptr,16);
    string value = r->getRegist(raddress);
    QString steps = ir.mid(3,1);
    int x = steps.toInt(nullptr,16);
    int len = value.length();
    x = x % len;

    for (int i = 0; i < x; i++) {

        char lastBit = value[len - 1];


        for (int j = len - 1; j > 0; j--) {
            value[j] = value[j - 1];
        }

        value[0] = lastBit;
    }

    r->setRegist(value, raddress ,ui);
}


bool CU::jump(string& IR,Register* r)
{
    QString ir = QString::fromStdString(IR);
    QString rcell = ir.mid(1,1);
    int raddress = rcell.toInt(nullptr,16);
    if(IR[0]=='B')
    {
        QString ir = QString::fromStdString(IR);
        QString rcell = ir.mid(1,1);
        int raddress = rcell.toInt(nullptr,16);
        if(r->getRegist(raddress) == r->getRegist(0))
        {
            return true;
        }
        else{
            return false;
        }
    }else{
        QString hexr = QString::fromStdString(r->getRegist(raddress));
        QString hex0 = QString::fromStdString(r->getRegist(0));
        int nr = hexr.toInt(nullptr,16);
        int n0 = hex0.toInt(nullptr,16);
        if(nr>127)
        {
            nr-=256;
        }
        if (n0>127){
            n0-=256;
        }
        if(nr > n0)
        {
            return true;
        }
        else{
            return false;
        }
    }
}



void MainWindow::on_store_clicked()
{
    QString input = (ui->input->text()).toUpper();
    input.remove(' ');
    if(!isValid(input))
    {
        ui->input->clear();
        QMessageBox::critical(this,"Error","your input wasn't valid please try again");
    }
    else
    {
        input.remove("0X");
        if(loadCounter>=256)
        {
            QMessageBox::critical(this,"Error","Memory is full, clear Memory then try again");
        }else
        {
            userInput=input.toStdString();
            myMachine->load(userInput,loadCounter,ui);
            loadCounter += 2 ;
            ui->input->clear();
        }
    }
}



void MainWindow::on_run_clicked()
{
    int i = loadCounter;
    while(i>0)
    {
        if(myMachine->accessCPU()->getIR()[0]=='C')
        {
            break;
        }
            myMachine->run(ui);
            i-=2;
    }
}



void MainWindow::on_clearMemory_clicked()
{
    clearMemory();
    loadCounter=0;
}



void MainWindow::on_resetCPU_clicked()
{
    resetCPU();
}

