#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidgetItem>

#include <iostream>
#include <vector>
#include <tuple>


#include "qcustomplot.h"

typedef std::tuple<std::string,int,int> tupla;
typedef std::vector<tupla> procesos;

typedef std::tuple<std::string,int,int,int,int,double> registro;
typedef std::vector<registro> procesado;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void Graficar();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

    void on_tableWidget_itemClicked(QTableWidgetItem *item);

    void on_pushButton_4_clicked();

    void on_pushButton_2_clicked();

    void mostrar();

    void mostrarNuevo();

    void procesando();

    //Moverse dentro del Grafico
    void horzScrollBarChanged(int value);
    void vertScrollBarChanged(int value);
    void xAxisChanged(QCPRange range);
    void yAxisChanged(QCPRange range);


private:
    Ui::MainWindow *ui;

    int f;
    int F;
    std::string proc;
    int lleg;
    int serv;

    double R_promedio=0;
    double RN_promedio=0;

    procesos Procesos;
    procesado Procesado;

};

#endif // MAINWINDOW_H