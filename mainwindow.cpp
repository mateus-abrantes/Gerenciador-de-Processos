#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QDebug>
#include <QMessageBox>
#include <QTextStream>
#include <string.h>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    startTimer(100);
    connect(ui->lineEdit_filter,SIGNAL(textChanged(QString)),this,SLOT(set_filter()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    lista_processos(filter);
}

void MainWindow::set_filter()
{
    filter = ui->lineEdit_filter->text();
}

void MainWindow::lista_processos(QString _filter)
{
//    qDebug() << _filter;
    ui->listWidget_processos->clear();
    if(QString::compare(_filter,"") == 0){
        system("ps -ao pid,user,pri,nice,pcpu,pmem,time,comm -U $USER --sort=-pcpu | head -n 30 > processos.txt");
    }else{
        std::string filt = _filter.toStdString();
        const char* p = filt.c_str();
        system(("ps -ao pid,user,pri,nice,pcpu,pmem,time,comm -U $USER --sort=-pcpu | grep  '" + filt + "' > processos.txt").c_str());
    }
    QFile file("processos.txt");
    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(0, "error", file.errorString());
    }

    QTextStream in(&file);

    while(!in.atEnd()) {
        QString line = in.readLine();
//        QStringList fields = line.split(",");
//        qDebug() << line;
        ui->listWidget_processos->addItem(line);
    }

    file.close();
}


