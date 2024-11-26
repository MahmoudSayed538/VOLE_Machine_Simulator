#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<bits/stdc++.h>
#include<QTableWidget>
#include<QMessageBox>
#include<QApplication>
#include<QObject>

using namespace std;


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    void clearMemory();
    void resetCPU();
    string getUserInput();
    bool isValid(const QString& input);
    Ui::MainWindow* getUI()
    {
        return ui;
    }
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_store_clicked();

    void on_run_clicked();

    void on_clearMemory_clicked();

    void on_resetCPU_clicked();

private:
    Ui::MainWindow *ui;
    string userInput;
    int loadCounter;
};

class Memory
{
    vector<string> mainMemory;
public:
    Memory()
    {
        mainMemory.assign(256,"00");
    }
    void setMemory(const int& address,const string& value,Ui::MainWindow* ui);
    string getMemory(const int& address)
    {
        return mainMemory.at(address);
    }
};

class Register
{
    vector<string> regist;
public:
    Register()
    {
        regist.assign(16,"00");
    }
    void setRegist(const string& value , const int& address , Ui::MainWindow* ui);
    string getRegist(const int& address)
    {
        return regist.at(address);
    }
};

class CU{
public:
    void copy(const string& IR,Ui::MainWindow* ui,Memory* m,Register* r);
    void add(const string& IR,Ui::MainWindow* ui,Register* r);
    void OR(const string& IR,Ui::MainWindow* ui,Register* r);
    void AND(const string& IR,Ui::MainWindow* ui,Register* r);
    void XOR(const string& IR,Ui::MainWindow* ui,Register* r);
    void rotate(const string& IR,Ui::MainWindow*ui,Register* r);
    bool jump(string& IR,Register* r);
    void halt(Ui::MainWindow* ui);
};

class CPU {
    Register *r;
    int PC;
    string IR;
    CU cu;
public:
    CPU(int counter,string instruct)
    {
        PC = counter;
        IR = instruct;
        r= new Register();
    }
    void fetch(Memory* mainMemory,Ui::MainWindow* ui);
    void execute(Memory* mainMemory,Ui::MainWindow* ui);
    void setR(const string& value , const int& address , Ui::MainWindow* ui);
    void setIR(const string& value,Ui::MainWindow* ui);
    string getIR()
    {
        return IR;
    }
    void setPC(const int& value,Ui::MainWindow* ui);
};

class Machine
{
    Memory *m ;
    CPU *c ;
public:
    Machine()
    {
        m=new Memory();
        c = new CPU(0,"0000");
    }~Machine()
    {
        delete m;
        delete c;
    }
    void load(const string& input,const int& address,Ui::MainWindow* ui);
    void run(Ui::MainWindow* ui);
    CPU* accessCPU()
    {
        return c;
    }
    Memory* accessMemory()
    {
        return m;
    }
};

#endif // MAINWINDOW_H
