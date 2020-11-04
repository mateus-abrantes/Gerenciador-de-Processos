#include "plotter.h"
#include <unistd.h>
#include <QPainter>
#include <QBrush>
#include <QPen>
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <string>
#include <iostream>

Plotter::Plotter(QWidget *parent) : QWidget(parent)
{
    startTimer(100);

}

void Plotter::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QBrush brush;
    QPen pen;

    //Brush
    brush.setColor(QColor(255,255,255));
    brush.setStyle(Qt::SolidPattern);

    //Pen
    pen.setColor(QColor(0,0,0));
    pen.setWidth(2);

    //Painter
    painter.setBrush(brush);
    painter.setPen(pen);
    painter.drawRect(0,0,width(),height());
    //numero de nucleos
    long number_of_processors = sysconf(_SC_NPROCESSORS_ONLN);

    //Percentage use cpu
    system("ps axo pcpu,psr,pid > core_processos.txt");
    float acumulate[number_of_processors];
    //Inicializa o array das percetagens de uso da cpu em 0
    for(int i = 0; i< number_of_processors;i++){
        acumulate[i]=0;
    }
    QFile file("core_processos.txt");
    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(0, "error", file.errorString());
    }

    QTextStream in(&file);

    while(!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(" ");
        fields.removeAll(QString(""));
        for(int i = 0; i< number_of_processors;i++){
            if(QString::compare(fields[1],QString::number(i)) == 0){
                acumulate[i] += fields[0].toFloat();
            }
        }
    }

    file.close();

    //Painter graphics
    int x, y, largura;
    x = 0; y =0;
    largura = width()/number_of_processors;
    //Desenha os retangulos para o grafico de cada core
    for(int i = 1; i<= number_of_processors;i++){
        painter.drawRect(x,y,largura*i,height());
        x = largura*i;
        QString nome_core = QString("CORE ").append(QString::number(i-1));
        painter.drawText(((largura*(i-1))+largura/2)-20,20,nome_core);
    }
    //Desenha o nome de cada core e seu grafico
    for(int i = 1; i<= number_of_processors;i++){
        brush.setColor(QColor(255,255,0));
        brush.setStyle(Qt::SolidPattern);
        //Seta a cor para diferentes percentagens
        if((int)acumulate[i-1]>= 0 && (int)acumulate[i-1] <=30){
            brush.setColor(QColor(0,255,0));
        }else if((int)acumulate[i-1]> 30 && (int)acumulate[i-1] <=70){
            brush.setColor(QColor(0,0,255));
        }else if((int)acumulate[i-1]> 70){
            brush.setColor(QColor(255,0,0));
        }
        painter.setBrush(brush);
        QString nome_core = QString("CORE ").append(QString::number(i));
        painter.drawRect(((largura*(i-1))+largura/3),height()-(int)acumulate[i-1],largura/3,(int)acumulate[i-1]);
        painter.drawText(((largura*(i-1))+largura/2)-16,height()-(int)acumulate[i-1]-3,QString::number(acumulate[i-1])+"%");
        x = largura*i;
    }
}

void Plotter::timerEvent(QTimerEvent *event)
{
    repaint();
}
