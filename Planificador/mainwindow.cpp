#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    // configure scroll bars:
    // Since scroll bars only support integer values, we'll set a high default range of -500..500 and
    // divide scroll bar position values by 100 to provide a scroll range -5..5 in floating point
    // axis coordinates. if you want to dynamically grow the range accessible with the scroll bar,
    // just increase the the minimum/maximum values of the scroll bars as needed.
    ui->horizontalScrollBar->setRange(-500, 500);
    ui->verticalScrollBar->setRange(-500, 500);

    // create connection between axes and scroll bars:
    connect(ui->horizontalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(horzScrollBarChanged(int)));
    connect(ui->verticalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(vertScrollBarChanged(int)));
    connect(ui->Graph->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(xAxisChanged(QCPRange)));
    connect(ui->Graph->yAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(yAxisChanged(QCPRange)));

    // initialize axis range (and scroll bar positions via signals we just connected):
    ui->Graph->xAxis->setRange(0, 10, Qt::AlignCenter);
    ui->Graph->yAxis->setRange(0, 10, Qt::AlignCenter);
    ui->Graph->xAxis->setLabel("Tiempo");
    ui->Graph->yAxis->setLabel("Proceso");


    ui->tableWidget->setColumnCount(6);

    QStringList L;

    L << "Proceso" << "T. llegada" << "T. Servicio" << "T. finalización" <<"T. Retorno" << "T. R. Normalizado";
    ui->tableWidget->setHorizontalHeaderLabels(L);

    ui->tableWidget->setColumnWidth(0,80);
    ui->tableWidget->setColumnWidth(1,89);
    ui->tableWidget->setColumnWidth(2,89);
    ui->tableWidget->setColumnWidth(3,119);
    ui->tableWidget->setColumnWidth(4,89);
    ui->tableWidget->setColumnWidth(5,150);

    F=0;
    f=-1;



}

MainWindow::~MainWindow()
{
    delete ui;
}

//Funcion para Graficar.
void MainWindow::Graficar()
{
  // The following plot setup is mostly taken from the plot demos:
  ui->Graph->addGraph();
  ui->Graph->graph()->setPen(QPen(Qt::blue));
  //ui->Graph->graph()->setBrush(QBrush(QColor(0, 0, 255, 20)));

  //Funcion Encargada de Graficar
  QVector<double> x(10000), y(10000);

  x[0] = 0; y[0] = 0;

 for(unsigned long i = 1, j=0; j<Procesos.size(); ++i, ++j)
  {

     if( std::get<1>(Procesos[j]) < x[i-1] )
     {

         x[i] = std::get<2>(Procesos[j]) + x[i-1];
         y[i] = y[i-1] + 1;

     }

     else
          {

              x[i] = std::get<1>( Procesos[j] );
              y[i] = 0;

              x[i+1] = std::get<2>(Procesos[j]) + x[i];
              y[i+1] = y[i-1] + 1;

              ++i;
          }

  }



 //Nombre de los Procesos
 QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);

    textTicker->addTick(0, "Nada");
 for (unsigned long i = 1; i<=Procesos.size(); i++)
  {

     textTicker->addTick(i, QString::fromStdString( std::get<0>( Procesos[i-1] ) ) );

  }

  //Añadir nombre de procesos dentro de la gráfica
  ui->Graph->yAxis->setTicker(textTicker);

  //Añadiendo puntos a graficar.
  ui->Graph->graph(0)->setData(x,y);
  ui->Graph->graph(0)->setLineStyle(QCPGraph::lsStepRight);
  ui->Graph->replot();

  ui->Graph->axisRect()->setupFullAxesBox(true);
  ui->Graph->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

  //Borrar anterior operacion
  Procesado.clear();
  R_promedio = 0;
  RN_promedio = 0;
}


//INSERTAR
void MainWindow::on_pushButton_clicked()
{
    F = ui->tableWidget->rowCount();

    ui->tableWidget->insertRow(F);
    ui->tableWidget->setItem(F,0, new QTableWidgetItem(ui->lineEdit_3->text()));
    ui->tableWidget->setItem(F,1, new QTableWidgetItem(ui->lineEdit->text()));
    ui->tableWidget->setItem(F,2, new QTableWidgetItem(ui->lineEdit_2->text()));

    proc = ui->lineEdit_3->text().toStdString();
    lleg = ui->lineEdit->text().toInt();
    serv = ui->lineEdit_2->text().toInt();

    //Insertar en vector de procesos
    tupla nuevo =  tupla(proc,lleg,serv);
    Procesos.push_back(nuevo);

    ui->lineEdit->clear();
    ui->lineEdit_2->clear();
    ui->lineEdit_3->clear();
}

void MainWindow::mostrar(){
  for (auto i: Procesos){
    std::cout<<"Proceso:  "<<std::get<0>(i)<<std::endl;
      std::cout<<"    Tiempo de llegada: "<< std::get<1>(i)<<"  Tiempo de servicio: "<< std::get<2>(i)<<std::endl;
  }
}

void MainWindow::mostrarNuevo(){
  for (auto i: Procesado){
    std::cout<<"Proceso:  "<<std::get<0>(i)<<std::endl;
      std::cout<<"    Tiempo de llegada: "<< std::get<1>(i)<<"  Tiempo de servicio: "<< std::get<2>(i)<<
                 "  Tiempo finalizado: "<<std::get<3>(i)<<"  Tiempo de retorno: "<< std::get<4>(i)<<
                 "  Tiempo normalizado: "<<std::get<5>(i)<<std::endl;
  }
}

void MainWindow::procesando(){
    int fin = 0;
    int retorno = 0;
    int n = Procesos.size();
    double normalizado = 0;

    int ant_finalizado;

    bool primero = false;

    for (auto i: Procesos){
        if(primero == false){
            fin = std::get<2>(i) + std::get<1>(i);
            primero = true;
            ant_finalizado = fin;
        }
        else{
            fin = std::get<2>(i) + ant_finalizado;
            ant_finalizado = fin;
        }

        retorno = fin - std::get<1>(i);
        normalizado = retorno*1.0 /std::get<2>(i);
        R_promedio = R_promedio + retorno;
        RN_promedio = RN_promedio + normalizado;

        registro nuevo = registro(std::get<0>(i),std::get<1>(i),std::get<2>(i),fin,retorno, normalizado);
        Procesado.push_back(nuevo);
    }

    R_promedio = R_promedio *1.0 / n;
    RN_promedio = RN_promedio *1.0 / n;
}

void MainWindow::on_pushButton_3_clicked()
{
    Procesos.clear();
    Procesado.clear();
    R_promedio = 0;
    RN_promedio = 0;
    ui->tableWidget->setRowCount(0);
    //close();
}

//Al hacer click
void MainWindow::on_tableWidget_itemClicked(QTableWidgetItem *item)
{
    f = item->row();

    QTableWidgetItem *proceso = ui->tableWidget->item(f,0);
    QTableWidgetItem *llegada = ui->tableWidget->item(f,1);
    QTableWidgetItem *servicio = ui->tableWidget->item(f,2);

    ui->lineEdit_3->setText(proceso->text());
    ui->lineEdit->setText(llegada->text());
    ui->lineEdit_2->setText(servicio->text());
}

//Editar campo
void MainWindow::on_pushButton_4_clicked()
{
    ui->tableWidget->setItem(f,0,new QTableWidgetItem(ui->lineEdit_3->text()));
    ui->tableWidget->setItem(f,1,new QTableWidgetItem(ui->lineEdit->text()));
    ui->tableWidget->setItem(f,2,new QTableWidgetItem(ui->lineEdit_2->text()));

    proc = ui->lineEdit_3->text().toStdString();
    lleg = ui->lineEdit->text().toInt();
    serv = ui->lineEdit_2->text().toInt();

    //Edita en esa posicion al vector de procesos
    tupla nuevo =  tupla(proc,lleg,serv);
    Procesos.at(f) = nuevo;
}

void MainWindow::on_pushButton_2_clicked()
{
    mostrar();
    procesando();
    mostrarNuevo();
    std::cout<< "Tiempor de retorno promedio: "<<R_promedio<<std::endl;
    std::cout<< "Tiempo de retorno normalizado promedio: "<<RN_promedio<<std::endl;

    //CREANDO TABLA FINAL
    ui->tableWidget->setRowCount(0);   //resetea tabla
    int fin = 0;
    int retorno = 0;
    double normalizado = 0;
    F=0;

    for(auto i:Procesado)
    {
        proc = std::get<0>(i);
        lleg = std::get<1>(i);
        serv = std::get<2>(i);
        fin = std::get<3>(i);
        retorno = std::get<4>(i);
        normalizado = std::get<5>(i);

        ui->tableWidget->insertRow(F);
        ui->tableWidget->setItem(F,0, new QTableWidgetItem(QString::fromStdString(proc)));
        ui->tableWidget->setItem(F,1, new QTableWidgetItem(QString::number(lleg)));
        ui->tableWidget->setItem(F,2, new QTableWidgetItem(QString::number(serv)));
        ui->tableWidget->setItem(F,3, new QTableWidgetItem(QString::number(fin)));
        ui->tableWidget->setItem(F,4, new QTableWidgetItem(QString::number(retorno)));
        ui->tableWidget->setItem(F,5, new QTableWidgetItem(QString::number(normalizado)));
        F++;
    }

    ui->TRP->setText(QString::number(R_promedio));
    ui->TRNP->setText(QString::number(RN_promedio));


    Graficar();

}

void MainWindow::horzScrollBarChanged(int value)
{
  if (qAbs(ui->Graph->xAxis->range().center()-value/100.0) > 0.01) // if user is dragging plot, we don't want to replot twice
  {
    ui->Graph->xAxis->setRange(value/100.0, ui->Graph->xAxis->range().size(), Qt::AlignCenter);
    ui->Graph->replot();
  }
}

void MainWindow::vertScrollBarChanged(int value)
{
  if (qAbs(ui->Graph->yAxis->range().center()+value/100.0) > 0.01) // if user is dragging plot, we don't want to replot twice
  {
    ui->Graph->yAxis->setRange(-value/100.0, ui->Graph->yAxis->range().size(), Qt::AlignCenter);
    ui->Graph->replot();
  }
}

void MainWindow::xAxisChanged(QCPRange range)
{
  ui->horizontalScrollBar->setValue(qRound(range.center()*100.0)); // adjust position of scroll bar slider
  ui->horizontalScrollBar->setPageStep(qRound(range.size()*100.0)); // adjust size of scroll bar slider
}

void MainWindow::yAxisChanged(QCPRange range)
{
  ui->verticalScrollBar->setValue(qRound(-range.center()*100.0)); // adjust position of scroll bar slider
  ui->verticalScrollBar->setPageStep(qRound(range.size()*100.0)); // adjust size of scroll bar slider
}

