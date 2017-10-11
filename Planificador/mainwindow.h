#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidgetItem>

#include <iostream>
#include <vector>
#include <tuple>

#include <QTimer>

#include "qcustomplot.h"

typedef std::tuple<std::string,int,int> tupla;
typedef std::vector<tupla> procesos;
typedef std::vector<tupla>::iterator procesos_it;


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

    void Posiciones();

    void ImprimirVector(procesos vector);
    void ImprimirEncontrados(std::vector<procesos_it> &encontrados);

    std::vector<procesos_it> buscarEnVector(std::string nombre, int valori, int valor, procesos &vector);
    void eliminarEnVector(procesos &vector, std::vector<procesos_it> &vectorelim);
    void agregarCola(procesos &colaProcesos, std::vector<procesos_it> nuevosLlegados);

    void fifo();
    void primeroMasCorto();



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



    void realtimeDataSlot();




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

    procesos ProcesosTemporal;
    procesos colaProcesos;

    procesado Procesado;

    std::map <std::string, int> tInicio;
    std::map <std::string, int> tFinal;

    //Mapa de Posiciones.
    std::map < std::string, int > posiciones;

    QTimer dataTimer;

    int TimeElapsed = 0;
    int pos = 0;

    QVector<double> x1;
    QVector<double> y1;
    QVector<double> x2;
    QVector<double> y2;

};

#endif // MAINWINDOW_H
