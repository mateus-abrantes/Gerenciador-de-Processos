#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void timerEvent(QTimerEvent *event);
public slots:
    void lista_processos(QString _filter);
    void get_pid_item_list_process();
    void set_filter();
    void matar_processo();
    void parar_processo();
    void continuar_processo();
    void alterar_core();
    void alterar_prioridade();
private:
    Ui::MainWindow *ui;
    QString filter;
};
#endif // MAINWINDOW_H
