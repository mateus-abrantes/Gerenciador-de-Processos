#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QDebug>
#include <QMessageBox>
#include <QTextStream>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>   //kill
#include <sys/time.h> // getpriority(int which, int who)  setpriority(int which, int who, int prio);
#include <sys/resource.h>
#include <cstdlib> //in C++
#include <iostream>
#define _GNU_SOURCE
#include <sched.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //lista o numero de de CPU`S
    long number_of_processors = sysconf(_SC_NPROCESSORS_ONLN);
    for(int i=0;i<number_of_processors;i++){
        ui->comboBox_CPU->addItem(QString::number(i));
    }
    //Inicia o temporizador em 100 ms
    startTimer(100);

    //Filtrar processo
    connect(ui->listWidget_processos,SIGNAL(itemPressed(QListWidgetItem*)),this,SLOT(get_pid_item_list_process()));
    connect(ui->lineEdit_filter,SIGNAL(textChanged(QString)),this,SLOT(set_filter()));
    //Matar processo
    connect(ui->pushButton_MATAR,SIGNAL(clicked(bool)),this,SLOT(matar_processo()));
    //Parar processo
    connect(ui->pushButton_SUSPENDER,SIGNAL(clicked(bool)),this,SLOT(parar_processo()));
    //Continuar processo
    connect(ui->pushButton_CONTINUAR,SIGNAL(clicked(bool)),this,SLOT(continuar_processo()));
    //Alterar core a ser executado o processo
    connect(ui->pushButton_ALTERAR_CPU,SIGNAL(clicked(bool)),this,SLOT(alterar_core()));

    setWindowTitle("Gerenciador de Processos");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    //Lista os processos
    lista_processos(filter);
}



void MainWindow::set_filter()
{
    //Define o filtro de processo
    filter = ui->lineEdit_filter->text();
}

void MainWindow::matar_processo()
{
    QString instrucao_kill;
    QString valor = ui->lineEdit_PID->text();
    if(QString::compare(valor,"") == 0){
        QMessageBox::information(0, "Erro","Nenhum PID foi digitado");
    }else{
        bool number;
        int dec = valor.toInt(&number, 10);
        if(number == false){
            QMessageBox::information(0, "Erro","O PID tem que ser numerico");
        }else{
            instrucao_kill = kill(valor.toInt(), SIGKILL);
            std::string instrucao = instrucao_kill.toStdString();
            system(instrucao.c_str());
            QMessageBox::information(0, "Alerta","O processo de PID:"+valor+" foi morto!");
            ui->lineEdit_PID->setText("");
        }
    }
}

void MainWindow::parar_processo()
{
    QString instrucao_parar;
    QString valor = ui->lineEdit_PID->text();
    if(QString::compare(valor,"") == 0){
        QMessageBox::information(0, "Erro","Nenhum PID foi digitado");
    }else{
        bool number;
        int dec = valor.toInt(&number, 10);
        if(number == false){
            QMessageBox::information(0, "Erro","O PID tem que ser numerico");
        }else{
            instrucao_parar = kill(valor.toInt(), SIGSTOP);
            std::string instrucao = instrucao_parar.toStdString();
            system(instrucao.c_str());
            QMessageBox::information(0, "Alerta","O processo de PID:"+valor+" foi parado!");
        }
    }
}

void MainWindow::continuar_processo()
{
    QString instrucao_continuar;
    QString valor = ui->lineEdit_PID->text();
    if(QString::compare(valor,"") == 0){
        QMessageBox::information(0, "Erro","Nenhum PID foi digitado");
    }else{
        bool number;
        int dec = valor.toInt(&number, 10);
        if(number == false){
            QMessageBox::information(0, "Erro","O PID tem que ser numerico");
        }else{
            instrucao_continuar = kill(valor.toInt(), SIGCONT);
            std::string instrucao = instrucao_continuar.toStdString();
            system(instrucao.c_str());
            QMessageBox::information(0, "Alerta","O processo de PID:"+valor+" foi continuado!");
        }
    }
}

void MainWindow::alterar_core()
{
    int core_val,pid_val;
    QString valor_selecionado,valor_pid;
    valor_selecionado = ui->comboBox_CPU->currentText();
    core_val = valor_selecionado.toInt();
    valor_pid = ui->lineEdit_PID->text();
    if(QString::compare(valor_pid,"") == 0){
        QMessageBox::information(0, "Erro","Nenhum PID foi digitado");
    }else{
        bool number;
        int dec = valor_pid.toInt(&number, 10);
        if(number == false){
            QMessageBox::information(0, "Erro","O PID tem que ser numerico");
        }else{
            pid_val = valor_pid.toInt();
            cpu_set_t mask;
            CPU_ZERO(&mask);
            CPU_SET(core_val, &mask);
            sched_setaffinity(pid_val, sizeof(mask), &mask);
            QMessageBox::information(0, "Alerta","O processo de PID:"+valor_pid+" foi alocado no core "+valor_selecionado+" !");
        }
    }
}

void MainWindow::lista_processos(QString _filter)
{
    //    qDebug() << _filter;
    ui->listWidget_processos->clear();
    if(QString::compare(_filter,"") == 0){
        system("ps -ao pid,user,pri,stat,nice,psr,pcpu,pmem,time,comm -U $USER --sort=-pcpu | head -n 30 > processos.txt");
    }else{
        std::string filt = _filter.toStdString();
        system(("ps -ao pid,user,pri,stat,nice,psr,pcpu,pmem,time,comm -U $USER --sort=-pcpu | grep  '" + filt + "' > processos.txt").c_str());
    }
    QFile file("processos.txt");
    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(0, "error", file.errorString());
    }

    QTextStream in(&file);

    while(!in.atEnd()) {
        QString line = in.readLine();
        ui->listWidget_processos->addItem(line);
    }

    file.close();
}

void MainWindow::get_pid_item_list_process()
{
    QString a = ui->listWidget_processos->currentItem()->text();
    QStringList aux = a.split(" ");
    aux.removeDuplicates();
    if(QString::compare(aux[1],"PID") != 0){
        ui->lineEdit_PID->setText(aux[1]);
    }
}


