#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTimer>

using namespace std;

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

    ui->Graph->xAxis->setLabel("Tiempo");
    ui->Graph->yAxis->setLabel("Proceso");

    ui->Graph->yAxis->setRange(-1,5);
    ui->Graph->xAxis->setRange(-1,7);

    //x1.resize(10000); x2.resize(10000); y1.resize(10000); y2.resize(10000);

    ui->tableWidget->setColumnCount(9);

    QStringList L;

    L << "Proceso" << "T. llegada" << "T. Servicio" << "T. finalización" <<"T. Retorno" << "T. R. Normalizado" << "T.Entrada"<<"T.Espera"<<"Prioridad";
    ui->tableWidget->setHorizontalHeaderLabels(L);

    ui->tableWidget->setColumnWidth(0,65);
    ui->tableWidget->setColumnWidth(1,75);
    ui->tableWidget->setColumnWidth(2,75);
    ui->tableWidget->setColumnWidth(3,100);
    ui->tableWidget->setColumnWidth(4,75);
    ui->tableWidget->setColumnWidth(5,128);
    ui->tableWidget->setColumnWidth(6,71);
    ui->tableWidget->setColumnWidth(7,71);
    ui->tableWidget->setColumnWidth(8,71);

    F=0;
    f=-1;
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::Posiciones()
{
    //Introducimos las posiciones.
    for( unsigned long i = 0; i<Procesos.size(); ++i )
    {
        posiciones.insert( std::pair<std::string,int>(std::get<0>( Procesos[i] ) , i+1) );
    }
}

void MainWindow::PosicionesP()
{
    //Introducimos las posiciones.
    for( unsigned long i = 0; i<ProcesosP.size(); ++i )
    {
        posiciones.insert( std::pair<std::string,int>(std::get<0>( ProcesosP[i] ) , i+1) );
    }
}



void MainWindow::ImprimirVector(procesos vector)
{

    for(auto i: vector)
    {

        std::cout << "Nombre Proceso: " << std::get<0>(i) << std::endl;
        //std::cout  << " Tiempo llegada: " << std::get<1>(i) << std::endl;
        //std::cout << " Tiempo de Servicio: " << std::get<2>(i) << std::endl;

    }

}

void MainWindow::ImprimirEncontrados(std::vector<procesos_it> &encontrados)
{

    std::cout<< "Los procesos encontrados: " << std::endl;
    for(auto i: encontrados)
    {


        std::cout << " " <<std::get<0>(*i) << std::endl;
    }
    std::cout << std::endl;
}




std::vector<procesos_it> MainWindow::buscarEnVector(std::string nombre, int valori, int valorf, procesos &vector)
{

    std::vector<procesos_it> nuevosCola;
    std::cout << "Desde: " << valori << " hasta: " << valorf << std::endl;
    std::cout << "Procesos que llegaron/encontrados:" << std::endl;

    for(procesos_it it = vector.begin(); it != vector.end(); it++ )
    {
        if( std::get<1>(*it) >= valori  && std::get<1>(*it) <= valorf && std::get<0>(*it) != nombre && buscarEnColaProcesos(std::get<0>(*it)) == false )
        {
            std::cout << " " << std::get<0>(*it) << std::endl;
            nuevosCola.push_back(it);
        }
    }

    return nuevosCola;
}

void MainWindow::eliminarEnVector(procesos &vector, std::vector<procesos_it> &vectorelim)
{

    std::cout << "Procesos a eliminar: " << std::endl;

    for(auto i:vectorelim)
    {
        std::cout <<" " << std::get<0>(*i);
        vector.erase(i);
    }

    std::cout << std::endl;
}


void MainWindow::agregarCola(procesos &colaProcesos, std::vector<procesos_it> nuevosLlegados)
{

    std::cout << "Procesos a agregar a la cola: " << std::endl;

    for(auto i: nuevosLlegados)
    {
        std::cout <<" " << std::get<0>(*i);
        colaProcesos.push_back(*i);
    }
    std::cout << std::endl;
}

bool MainWindow::buscarEnTiempo(int tiempo, tupla &buscando, procesos &Procs)
{

    for (unsigned long i = 0; i<Procs.size(); i++)
    {
        if(std::get<1>(Procs[i]) == tiempo )
        {
            buscando = Procs[i];
            std:: cout << "Proceso encontrado en tiempo "<< tiempo << std::endl;
            Procs.erase(Procs.begin() + i);
            return true;
        }
    }
    std::cout << "Proceso no encontrado en tiempo " << tiempo << std::endl;
    return false;
}

bool MainWindow::buscarVariosEnTiempo(int tiempo, std::vector<tuplaP> &buscando, procesosP &Procs)
{

    for (unsigned long i = 0; i<Procs.size(); i++)
    {
        if(std::get<1>(Procs[i]) == tiempo )
        {
            buscando.push_back(Procs[i]);
            std:: cout << "Proceso encontrado en tiempo "<< tiempo << std::endl;
            Procs.erase(Procs.begin() + i);
            i--;

        }
    }

    if(buscando.size() != 0)
    {
        cout << "entra" << endl;
        return true;
    }

    std::cout << "Proceso no encontrado en tiempo " << tiempo << std::endl;
    return false;

}

tupla MainWindow::buscarPorNombre(std::string nombre)
{
    tupla encontrado;
    for(auto i: Procesos)
    {

        if( std::get<0>(i) == nombre)
        {
            encontrado = i;
        }

    }

    return encontrado;
}

tuplaP MainWindow::buscarPorNombreP(string nombre)
{

    tuplaP encontrado;
    for(auto i: ProcesosP)
    {

        if( std::get<0>(i) == nombre)
        {
            encontrado = i;
        }

    }

    return encontrado;

}

bool MainWindow::buscarEnColaProcesos(string nombre)
{

    for(auto i: colaProcesos)
    {

        if(std::get<0>(i) == nombre)
            return true;
    }

    return false;
}

void MainWindow::imprimirColaProcesos()
{

    cout<< "Cola de Procesos:" << endl;
    for(auto i: colaProcesos)
    {

        cout << get<0>(i) << endl;

    }
    cout << endl;

}


void MainWindow::fifo()
{

    x1.push_back(0); y1.push_back(0);


    for(auto i: Procesos)
    {

        if( std::get<1>(i) <= x1.last() )
        {

            double tmp = x1.last() + 1;
            double tmp2 = std::get<2>(i) + x1.last();

            tInicio.insert( std::pair<std::string,int>(std::get<0>( i ) , tmp-1) );

            while( tmp <= tmp2 )
            {

                x1.push_back(tmp);
                y1.push_back( posiciones.find(std::get<0>(i))->second  );
                tmp++;
            }
            tFinal.insert( std::pair<std::string,int>(std::get<0>( i ) , tmp2) );

        }
        else
        {

            double tmp = x1.last() + 1;
            double tmp2 = std::get<1>(i) + x1.last();

            while( tmp <= tmp2)
            {

                x1.push_back(tmp );
                y1.push_back(0);
                tmp++;
            }

            tmp = x1.last() + 1;
            tmp2 = std::get<2>(i) + x1.last();

            tInicio.insert( std::pair<std::string,int>(std::get<0>( i ) , tmp-1) );

            while(tmp <= tmp2)
            {

                x1.push_back(tmp);
                y1.push_back(posiciones.find( std::get<0>( i ) )->second);

                tmp++;
            }
            tFinal.insert( std::pair<std::string,int>(std::get<0>( i ) , tmp2) );
        }

    }


    std::cout << "Inicio:" << std::endl;
    for(auto i: tInicio )
    {

        std::cout << i.first << "-" << i.second << std::endl;
    }
    std::cout << "Final:" << std::endl;
    for(auto i: tFinal)
    {

        std::cout << i.first << "-" << i.second << std::endl;
    }

}

void MainWindow::primeroMasCorto()
{
    std::string nombre;
    x1.push_back(0); y1.push_back(0);
    ProcesosTemporal = Procesos;

    colaProcesos.clear();

    colaProcesos.push_back(ProcesosTemporal.front());
    ProcesosTemporal.erase(ProcesosTemporal.begin());

    std::cout << "Procesos: " << std::endl;
    ImprimirVector(ProcesosTemporal);

    std::cout << "---" << std::endl;


    while( colaProcesos.size() != 0)
    {

        if( std::get<1>(colaProcesos.front()) <= x1.last() )
        {
            std::cout << "entra a graficar el proceso: " << std::get<0>(colaProcesos.front()) << std::endl;
            double tmp = x1.last() + 1;
            double tmp2 = std::get<2>(colaProcesos.front()) + x1.last();
            tInicio.insert( std::pair<std::string,int>(std::get<0>( colaProcesos.front() ) , tmp-1) );
            while( tmp <= tmp2 )
            {

                x1.push_back(tmp);
                y1.push_back( posiciones.find(std::get<0>(colaProcesos.front()))->second  );
                tmp++;
            }

            tFinal.insert( std::pair<std::string,int>(std::get<0>( colaProcesos.front() ) , tmp2));

        }
        else
        {
            std::cout << "entra a graficar el proc: " << std::get<0>(colaProcesos.front()) << std::endl;
            double tmp = x1.last() + 1;
            double tmp2 = std::get<1>(colaProcesos.front()) + x1.last();

            while( tmp <= tmp2)
            {

                x1.push_back(tmp );
                y1.push_back(0);
                tmp++;
            }

            tmp = x1.last() + 1;
            tmp2 = std::get<2>(colaProcesos.front()) + x1.last();
            tInicio.insert( std::pair<std::string,int>(std::get<0>( colaProcesos.front() ) , tmp-1) );
            while(tmp <= tmp2)
            {

                x1.push_back(tmp);
                y1.push_back(posiciones.find( std::get<0>( colaProcesos.front() ) )->second);

                tmp++;
            }
            tFinal.insert( std::pair<std::string,int>(std::get<0>( colaProcesos.front() ) , tmp2));

        }

        nombre = std::get<0>(colaProcesos.front());

        imprimirColaProcesos();

        std::vector<procesos_it> nuevosLlegados = buscarEnVector(nombre , tInicio.find( nombre )->second ,x1.last(),ProcesosTemporal);


        colaProcesos.erase(colaProcesos.begin());

        agregarCola( colaProcesos, nuevosLlegados );

        std::sort(colaProcesos.begin(), colaProcesos.end(),[]( tupla a, tupla b){
              return std::get<2>(a) <  std::get<2>(b);
        });


    }


    std::cout << "Inicio:" << std::endl;
    for(auto i: tInicio )
    {

        std::cout << i.first << "-" << i.second << std::endl;
    }
    std::cout << "Final:" << std::endl;
    for(auto i: tFinal)
    {

        std::cout << i.first << "-" << i.second << std::endl;
    }


}

void MainWindow::sjf_Expulsion()
{


    x1.push_back(0); y1.push_back(0);

    ProcesosTemporal = Procesos;

    colaProcesos.clear();



    std::cout << "Procesos: " << std::endl;
    ImprimirVector(ProcesosTemporal);

    std::cout << "---" << std::endl;

    int tiempo=0;
    int cantProc = Procesos.size();

    std::cout << "Entrando SRT" << std::endl;

    while( cantProc !=0 )
    {

        tupla buscando;
        std::cout <<"Tiempo: " << tiempo << std::endl;

        if( buscarEnTiempo(tiempo,buscando, ProcesosTemporal) )
        {


            if(colaProcesos.size() == 0)
            {
                colaProcesos.push_back(buscando);
            }
            else
            {

                if(std::get<2>(buscando) < std::get<2>(colaProcesos.front()))
                {

                    colaProcesos.push_back(colaProcesos.front());
                    colaProcesos.front() = buscando;

                    std::sort(colaProcesos.begin(), colaProcesos.end(),[]( tupla a, tupla b){
                          return std::get<2>(a) <  std::get<2>(b);
                    });


                }
                else
                {
                    colaProcesos.push_back(buscando);

                    std::sort(colaProcesos.begin(), colaProcesos.end(),[]( tupla a, tupla b){
                          return std::get<2>(a) <  std::get<2>(b);
                    });

                }
            }



        }
        else
        {

            if( colaProcesos.size() == 0 )
            {

                x1.push_back(x1.last()+1);
                y1.push_back(0);
                std::cout<< "["<<x1.last() << "," << y1.last() << "]" << std::endl;
                tiempo++;

            }
            else
            {

                if( std::get<2>(colaProcesos.front()) == 0)
                {


                    tFinal.insert( std::pair<std::string,int>(std::get<0>( colaProcesos.front() ) , tiempo));
                    colaProcesos.erase(colaProcesos.begin());
                    cantProc--;

                }
                else
                {
                    x1.push_back(x1.last()+1);
                    y1.push_back(posiciones.find( std::get<0>( colaProcesos.front() ) )->second);
                    std::cout<< "["<<x1.last() << "," << y1.last() << "]" << std::endl;

                    //Corregir verificación pesada
                    if(std::get<2>(colaProcesos.front()) == std::get<2>( buscarPorNombre( std::get<0>(colaProcesos.front()) )  ))
                        tInicio.insert( std::pair<std::string,int>(std::get<0>( colaProcesos.front() ) , tiempo) );

                    std::get<2>(colaProcesos.front())-= 1;
                    tiempo++;

                }


            }

        }


    }


    std::cout << "Inicio:" << std::endl;
    for(auto i: tInicio )
    {

        std::cout << i.first << "-" << i.second << std::endl;
    }
    std::cout << "Final:" << std::endl;
    for(auto i: tFinal)
    {

        std::cout << i.first << "-" << i.second << std::endl;
    }



}

void MainWindow::round_Robin()
{

    x1.push_back(0); y1.push_back(0);

    ProcesosTemporal = Procesos;

    colaProcesos.clear();



    std::cout << "Procesos: " << std::endl;
    ImprimirVector(ProcesosTemporal);

    std::cout << "---" << std::endl;

    int tiempo=0;
    int quantum2 = quantum;

    int cantProc = Procesos.size();

    std::cout << "Entrando Round Robin" << std::endl;


    while( cantProc !=0 )
    {

        tupla buscando;
        if( buscarEnTiempo(tiempo,buscando,ProcesosTemporal) )
        {

            colaProcesos.push_back(buscando);
            //cantProc++;

        }


        if( colaProcesos.size() == 0 )
        {

            x1.push_back(x1.last()+1);
            y1.push_back(0);
            cout << "[" << x1.last() << "," << y1.last() << "]" << endl;
            tiempo++;


        }
        else
        {


            if(get<2>(colaProcesos.front()) == 0)
            {

                tFinal.insert( std::pair<std::string,int>(std::get<0>( colaProcesos.front() ) , tiempo));
                cout << "El proceso " << get<0>( colaProcesos.front() ) << " ha terminado en el tiempo " << tiempo << endl;
                colaProcesos.erase( colaProcesos.begin() );

                cantProc--;
                quantum2= quantum;

            }
            else
            {

                if( quantum2 == 0)
                {

                    colaProcesos.push_back(colaProcesos.front());
                    colaProcesos.erase(colaProcesos.begin());
                    quantum2 = quantum;

                }
                else
                {

                    //Corregir verificación pesada
                    if(std::get<2>(colaProcesos.front()) == std::get<2>( buscarPorNombre( std::get<0>(colaProcesos.front()) )  ))
                        tInicio.insert( std::pair<std::string,int>(std::get<0>( colaProcesos.front() ) , tiempo) );

                    x1.push_back(x1.last()+1);
                    y1.push_back(posiciones.find( std::get<0>( colaProcesos.front() ) )->second);
                    cout << "Proceso: "<< get<0>( colaProcesos.front() ) << "[" << x1.last() << "," << y1.last() << "]" << endl;
                    get<2>(colaProcesos.front())-= 1;
                    tiempo++;
                    quantum2--;

                }



            }

        }


    }


    std::cout << "Inicio:" << std::endl;
    for(auto i: tInicio )
    {

        std::cout << i.first << "-" << i.second << std::endl;
    }
    std::cout << "Final:" << std::endl;
    for(auto i: tFinal)
    {

        std::cout << i.first << "-" << i.second << std::endl;
    }



}

void MainWindow::prioridad()
{

    x1.push_back(0); y1.push_back(0);

    ProcesosTemporalP = ProcesosP;

    colaProcesosP.clear();


    std::cout << "---" << std::endl;

    int tiempo=0;

    int cantProc = ProcesosP.size();

    std::cout << "Entrando Prioridad" << std::endl;

    while(cantProc != 0)
    {


        vector<tuplaP> buscando;
        if( buscarVariosEnTiempo(tiempo,buscando,ProcesosTemporalP) )
        {

            colaProcesosP.insert(colaProcesosP.end(), buscando.begin(), buscando.end());


            std::sort(colaProcesosP.begin(), colaProcesosP.end(),[]( tuplaP a, tuplaP b){
                  return std::get<3>(a) <  std::get<3>(b);
            });

            //cantProc++;

        }

        if( colaProcesosP.size() == 0 )
        {

            x1.push_back(x1.last()+1);
            y1.push_back(0);
            cout << "[" << x1.last() << "," << y1.last() << "]" << endl;
            tiempo++;


        }
        else
        {

            if(get<2>(colaProcesosP.front()) == 0)
            {

                tFinal.insert( std::pair<std::string,int>(std::get<0>( colaProcesosP.front() ) , tiempo));
                cout << "El proceso " << get<0>( colaProcesosP.front() ) << " ha terminado en el tiempo " << tiempo << endl;
                colaProcesosP.erase( colaProcesosP.begin() );

                cantProc--;

            }
            else
            {

                  //Corregir verificación pesada
                    if(std::get<2>(colaProcesosP.front()) == std::get<2>( buscarPorNombreP( std::get<0>(colaProcesosP.front()) )  ))
                        tInicio.insert( std::pair<std::string,int>(std::get<0>( colaProcesosP.front() ) , tiempo) );

                    x1.push_back(x1.last()+1);
                    y1.push_back(posiciones.find( std::get<0>( colaProcesosP.front() ) )->second);
                    cout << "Proceso: "<< get<0>( colaProcesosP.front() ) << "[" << x1.last() << "," << y1.last() << "]" << endl;
                    get<2>(colaProcesosP.front())-= 1;
                    tiempo++;
            }




        }




    }


}




//FUNCION KAT
//Funcion para Graficar.
void MainWindow::Graficar()
{

    // The following plot setup is mostly taken from the plot demos:
    ui->Graph->addGraph();
    ui->Graph->graph()->setPen(QPen(Qt::blue));
    //ui->Graph->graph()->setBrush(QBrush(QColor(0, 0, 255, 20)));

    ui->Graph->graph(0)->data()->clear();
    ui->Graph->replot();

    //Vectores de puntos.
    //QVector<double> x(10000), y(10000); x1, y1

    pos = 0;

    //Iniciando vector de puntos.

    //Nombre de los Procesos
    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    textTicker->addTick(0, "Nada");

    for( auto i: posiciones )
    {
    textTicker->addTick( i.second, QString::fromStdString(i.first)  );
    }

    ui->Graph->yAxis->setTicker(textTicker);

    //Tiempos de llegada
    QSharedPointer<QCPAxisTickerText> textTickerTLL(new QCPAxisTickerText);
    textTickerTLL->addTick(0, "Inicio");

    for (unsigned long i = 1; i<=Procesos.size(); i++)
    {
      textTickerTLL->addTick(  std::get<1>( Procesos[i-1] ) , QString::fromStdString(std::to_string( std::get<1>( Procesos[i-1] )   ) + " \n Llega \n Proceso \n " + std::get<0>(Procesos[i-1])) );
    }

    for (unsigned long i = 1; i<=ProcesosP.size(); i++)
    {
      textTickerTLL->addTick(  std::get<1>( ProcesosP[i-1] ) , QString::fromStdString(std::to_string( std::get<1>( ProcesosP[i-1] )   ) + " \n Llega \n Proceso \n " + std::get<0>(ProcesosP[i-1])) );
    }


    ui->Graph->xAxis->setTicker(textTickerTLL);
    ui->Graph->xAxis2->setVisible(true);
    ui->Graph->xAxis2->setTickLabels(true);
    ui->Graph->xAxis->setSubTicks(true);


    ui->Graph->axisRect()->setupFullAxesBox(true);
    ui->Graph->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    //Borrar anterior operacion
    Procesado.clear();
    R_promedio = 0;
    RN_promedio = 0;

    // make left and bottom axes transfer their ranges to right and top axes:
    connect(ui->Graph->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->Graph->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->Graph->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->Graph->yAxis2, SLOT(setRange(QCPRange)));

    // setup a timer that repeatedly calls MainWindow::realtimeDataSlot:
    connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
    dataTimer.start(0); // Interval 0 means to refresh as fast as possible

    //ui->Graph->graph(0)->setData(x1,y1);
    //ui->Graph->graph(0)->setLineStyle(QCPGraph::lsStepRight);

    //ui->Graph->replot();

}

void MainWindow::realtimeDataSlot()
{
  static QTime time(QTime::currentTime());
  // calculate two new data points:
  double key = time.elapsed(); // time elapsed since start of demo, in seconds

  if (key - TimeElapsed > 1000) // at most add point every 2 ms
  {
    // add data to lines:



    //std:: cout << key << " - "  << lastPointKey <<" = " << key - lastPointKey << std:: endl;
    //std:: cout << x1[pos] << "-" << y1[pos] << std::endl;
    x2.push_back(x1[0]);
    y2.push_back(y1[0]);
    x1.pop_front();
    y1.pop_front();

    // rescale value (vertical) axis to fit the current data:
    //ui->Graph->graph(0)->rescaleValueAxis();
    //ui->Graph->graph(1)->rescaleValueAxis(true);
    //std:: cout << "tam: "<< x1.size() << std:: endl;
    TimeElapsed = key;

    if(x1.size() == 0) {


        dataTimer.stop();
    }

    pos++;

  }



  // make key axis range scroll with the data (at a constant range size of 8):
  //Añadiendo puntos a graficar.
  ui->Graph->graph(0)->setData(x2,y2);
  ui->Graph->graph(0)->setLineStyle(QCPGraph::lsStepRight);
  ui->Graph->xAxis->setRange(pos, 8, Qt::AlignRight);

  ui->Graph->replot();


}

// FUNCION KAT
//INSERTAR
void MainWindow::on_pushButton_clicked()
{
    F = ui->tableWidget->rowCount();
    int index = ui->comboBox->currentIndex();

    ui->tableWidget->insertRow(F);
    ui->tableWidget->setItem(F,0, new QTableWidgetItem(ui->lineEdit_3->text()));
    ui->tableWidget->setItem(F,1, new QTableWidgetItem(ui->lineEdit->text()));
    ui->tableWidget->setItem(F,2, new QTableWidgetItem(ui->lineEdit_2->text()));


    proc = ui->lineEdit_3->text().toStdString();
    lleg = ui->lineEdit->text().toInt();
    serv = ui->lineEdit_2->text().toInt();
    prior = ui->lineEdit_5->text().toInt();

    // OBTIENE QUANTUM
    quantum = ui->lineEdit_4->text().toInt();
    cout<<"Quantum: " << quantum<<endl;

    //Insertar en vector de procesos
    if(index == 4){
        cout<<"Por Prioridad"<<endl;
        ui->tableWidget->setItem(F,8, new QTableWidgetItem(ui->lineEdit_5->text()));
        tuplaP nuevo= tuplaP(proc,lleg,serv,prior);
        ProcesosP.push_back(nuevo);
    }
    else
    {
        tupla nuevo =  tupla(proc,lleg,serv);
        Procesos.push_back(nuevo);
    }

    ui->lineEdit->clear();
    ui->lineEdit_2->clear();
    ui->lineEdit_3->clear();
    //ui->lineEdit_4->clear();
    ui->lineEdit_5->clear();
}

// FUNCION KAT
void MainWindow::mostrar(){
  for (auto i: Procesos){
    std::cout<<"Proceso:  "<<std::get<0>(i)<<std::endl;
      std::cout<<"    Tiempo de llegada: "<< std::get<1>(i)<<"  Tiempo de servicio: "<< std::get<2>(i)<<std::endl;
  }
}

void imprimir(procesos pro){
  for (auto i: pro){
    std::cout<<"Proceso:  "<<std::get<0>(i)<<std::endl;
      std::cout<<"    Tiempo de llegada: "<< std::get<1>(i)<<"  Tiempo de servicio: "<< std::get<2>(i)<<std::endl;
  }
}

// FUNCION KAT  //IMPRIMIR FIFO
void MainWindow::mostrarNuevo(){
  for (auto i: Procesado)
  {
    std::cout<<"Proceso:  "<<std::get<0>(i)<<std::endl;
      std::cout<<"    Tiempo de llegada: "<< std::get<1>(i)<<"  Tiempo de servicio: "<< std::get<2>(i)<<
                 "  Tiempo finalizado: "<<std::get<3>(i)<<"  Tiempo de retorno: "<< std::get<4>(i)<<
                 "  Tiempo normalizado: "<<std::get<5>(i)<<std::endl;
  }
}

// FUNCION KAT   //IMPRIMIR SPN o SRT
void MainWindow::mostrarNuevo2(){
  for (auto i: SRT)
  {
    std::cout<<"Proceso:  "<<std::get<0>(i)<<std::endl;
      std::cout<<"    Tiempo de llegada: "<< std::get<1>(i)<<"  Tiempo de servicio: "<< std::get<2>(i)<<
                 "  Tiempo entrada: "<<std::get<3>(i)<<"  Tiempo de espera: "<< std::get<4>(i)<<
                 "  Tiempo finalizacion: "<<std::get<5>(i)<<"  Tiempo retorno: "<<std::get<6>(i)<<
                 "  Tiempo normalizado: "<<std::get<6>(i)<<std::endl;
  }
}

// KAT
//FUNCION FIFO
void MainWindow::FIFO()
{
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

//KAT
// FUNCION PRIMERO MAS CORTO
void MainWindow::FuncionSPN()
{
    int fin = 0;
    int retorno = 0;
    int TEntrada = 0;
    int TEspera = 0;
    int n = Procesos.size();
    double normalizado = 0;

    for (auto i: Procesos){
        std::string nombre= std::get<0>(i);
        TEntrada = (tInicio.find(nombre))->second;
        TEspera = TEntrada - std::get<1>(i);
        fin = TEntrada + std::get<2>(i);
        retorno = fin - std::get<1>(i);
        normalizado = retorno*1.0 /std::get<2>(i);

        R_promedio = R_promedio + retorno;
        RN_promedio = RN_promedio + normalizado;

        otro nuevo = otro(std::get<0>(i),std::get<1>(i),std::get<2>(i),TEntrada,TEspera,fin,retorno, normalizado);
        SPN.push_back(nuevo);
    }

    R_promedio = R_promedio *1.0 / n;
    RN_promedio = RN_promedio *1.0 / n;
}

//KAT
//FUNCION SRT
void MainWindow::FuncionSRT()
{
    int fin = 0;
    int retorno = 0;
    int TEntrada = 0;
    int TEspera = 0;
    int n = Procesos.size();
    double normalizado = 0;

    std::cout<<"ENTRO!!\n";

    for ( auto i:Procesos)
    {
        std::string nombre= std::get<0>(i);
        TEntrada = (tInicio.find(nombre))->second;
        fin = (tFinal.find(nombre))->second;
        retorno = fin - std::get<1>(i);
        TEspera = retorno - std::get<2>(i);
        normalizado = retorno*1.0 /std::get<2>(i);

        R_promedio = R_promedio + retorno;
        RN_promedio = RN_promedio + normalizado;

        otro nuevo = otro(std::get<0>(i),std::get<1>(i),std::get<2>(i),TEntrada,TEspera,fin,retorno, normalizado);
        SRT.push_back(nuevo);
    }

    R_promedio = R_promedio * 1.0 / n;
    RN_promedio = RN_promedio * 1.0 / n;
}

//KAT
//FUNCION ROUND ROBIN
void MainWindow::FuncionRoundRobin()
{
    int fin = 0;
    int retorno = 0;
    int TEntrada = 0;
    int TEspera = 0;
    int n = Procesos.size();
    double normalizado = 0;

    for ( auto i:Procesos)
    {
        std::string nombre= std::get<0>(i);
        TEntrada = (tInicio.find(nombre))->second;
        cout<<"TEntrada: "<<TEntrada<<endl;
        fin = (tFinal.find(nombre))->second;
        cout<<"fin: "<<fin<<endl;
        retorno = fin - std::get<1>(i);
        TEspera = retorno - std::get<2>(i);
        normalizado = retorno*1.0 /std::get<2>(i);

        R_promedio = R_promedio + retorno;
        RN_promedio = RN_promedio + normalizado;

        otro nuevo = otro(std::get<0>(i),std::get<1>(i),std::get<2>(i),TEntrada,TEspera,fin,retorno, normalizado);
        RR.push_back(nuevo);
    }

    R_promedio = R_promedio * 1.0 / n;
    RN_promedio = RN_promedio * 1.0 / n;
}

//KAT
//FUNCION POR PRIORIDADES
void MainWindow::PorPrioridades()
{
    int fin = 0;
    int retorno = 0;
    int TEntrada = 0;
    int TEspera = 0;
    int Prioridad = 0;
    int n = ProcesosP.size();
    double normalizado = 0;

    for ( auto i:ProcesosP)
    {
        std::string nombre= std::get<0>(i);
        TEntrada = (tInicio.find(nombre))->second;
        cout<<"TEntrada: "<<TEntrada<<endl;
        fin = (tFinal.find(nombre))->second;
        cout<<"fin: "<<fin<<endl;
        retorno = fin - std::get<1>(i);
        TEspera = retorno - std::get<2>(i);
        Prioridad = std::get<3>(i);
        normalizado = retorno*1.0 /std::get<2>(i);

        R_promedio = R_promedio + retorno;
        RN_promedio = RN_promedio + normalizado;

        otroP nuevo = otroP(std::get<0>(i),std::get<1>(i),std::get<2>(i),TEntrada,TEspera,fin,retorno, normalizado,Prioridad);
        Pr.push_back(nuevo);
    }

    R_promedio = R_promedio * 1.0 / n;
    RN_promedio = RN_promedio * 1.0 / n;
}

// KAT
//Boton borrar todo
void MainWindow::on_pushButton_3_clicked()
{
    Procesos.clear();
    Procesado.clear();
    R_promedio = 0;
    RN_promedio = 0;
    ui->tableWidget->setRowCount(0);

    ui->Graph->graph(0)->data()->clear();
    ui->Graph->replot();

    x1.clear(); y1.clear(); x2.clear(); y2.clear();

    //close();
}

// KAT
//Al hacer click, se selecciona
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

// KAT
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

//BOTON GRAFICAR
void MainWindow::on_pushButton_2_clicked()
{
    int index = ui->comboBox->currentIndex();



    mostrar();
    if(index == 0)
    {
        Posiciones();
        fifo();
        FIFO();
        mostrarNuevo();
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
    }
    //SPN
    else if(index == 1)
    {
        Posiciones();
        primeroMasCorto();
        FuncionSPN();
        mostrarNuevo2();

        //CREANDO TABLA FINAL
        ui->tableWidget->setRowCount(0);   //resetea tabla
        int fin = 0;
        int retorno = 0;
        double normalizado = 0;
        F=0;

        for(auto i:SPN)
        {
            proc = std::get<0>(i);
            lleg = std::get<1>(i);
            serv = std::get<2>(i);
            entr = std::get<3>(i);
            esp = std::get<4>(i);
            fin = std::get<5>(i);
            retorno = std::get<6>(i);
            normalizado = std::get<7>(i);

            ui->tableWidget->insertRow(F);
            ui->tableWidget->setItem(F,0, new QTableWidgetItem(QString::fromStdString(proc)));
            ui->tableWidget->setItem(F,1, new QTableWidgetItem(QString::number(lleg)));
            ui->tableWidget->setItem(F,2, new QTableWidgetItem(QString::number(serv)));
            ui->tableWidget->setItem(F,3, new QTableWidgetItem(QString::number(fin)));
            ui->tableWidget->setItem(F,4, new QTableWidgetItem(QString::number(retorno)));
            ui->tableWidget->setItem(F,5, new QTableWidgetItem(QString::number(normalizado)));
            ui->tableWidget->setItem(F,6, new QTableWidgetItem(QString::number(entr)));
            ui->tableWidget->setItem(F,7, new QTableWidgetItem(QString::number(esp)));
            F++;
        }
    }
    //SRT
    else if( index == 2)
    {
        Posiciones();
        //Grafica
        sjf_Expulsion();
        // Tabla
        FuncionSRT();
        //Muestra en consola
        mostrarNuevo2();

        //CREANDO TABLA FINAL
        ui->tableWidget->setRowCount(0);   //resetea tabla
        int fin = 0;
        int retorno = 0;
        double normalizado = 0;
        F=0;

        for(auto i:SRT)
        {
            proc = std::get<0>(i);
            lleg = std::get<1>(i);
            serv = std::get<2>(i);
            entr = std::get<3>(i);
            esp = std::get<4>(i);
            fin = std::get<5>(i);
            retorno = std::get<6>(i);
            normalizado = std::get<7>(i);

            ui->tableWidget->insertRow(F);
            ui->tableWidget->setItem(F,0, new QTableWidgetItem(QString::fromStdString(proc)));
            ui->tableWidget->setItem(F,1, new QTableWidgetItem(QString::number(lleg)));
            ui->tableWidget->setItem(F,2, new QTableWidgetItem(QString::number(serv)));
            ui->tableWidget->setItem(F,3, new QTableWidgetItem(QString::number(fin)));
            ui->tableWidget->setItem(F,4, new QTableWidgetItem(QString::number(retorno)));
            ui->tableWidget->setItem(F,5, new QTableWidgetItem(QString::number(normalizado)));
            ui->tableWidget->setItem(F,6, new QTableWidgetItem(QString::number(entr)));
            ui->tableWidget->setItem(F,7, new QTableWidgetItem(QString::number(esp)));
            F++;
        }

    }

    else if(index == 3)
    {
        Posiciones();
        // Graficar
        round_Robin();
        // Tabla
        FuncionRoundRobin();
        //mostrarNuevo2();
        cout<<"FUNCION ROUND ROBIN"<<endl;

        ui->tableWidget->setRowCount(0);   //resetea tabla
        int fin = 0;
        int retorno = 0;
        double normalizado = 0;
        F=0;

        for(auto i:RR)
        {
            proc = std::get<0>(i);
            lleg = std::get<1>(i);
            serv = std::get<2>(i);
            entr = std::get<3>(i);
            esp = std::get<4>(i);
            fin = std::get<5>(i);
            retorno = std::get<6>(i);
            normalizado = std::get<7>(i);

            ui->tableWidget->insertRow(F);
            ui->tableWidget->setItem(F,0, new QTableWidgetItem(QString::fromStdString(proc)));
            ui->tableWidget->setItem(F,1, new QTableWidgetItem(QString::number(lleg)));
            ui->tableWidget->setItem(F,2, new QTableWidgetItem(QString::number(serv)));
            ui->tableWidget->setItem(F,3, new QTableWidgetItem(QString::number(fin)));
            ui->tableWidget->setItem(F,4, new QTableWidgetItem(QString::number(retorno)));
            ui->tableWidget->setItem(F,5, new QTableWidgetItem(QString::number(normalizado)));
            ui->tableWidget->setItem(F,6, new QTableWidgetItem(QString::number(entr)));
            ui->tableWidget->setItem(F,7, new QTableWidgetItem(QString::number(esp)));
            F++;
        }
    }
    else if(index == 4)
    {

        PosicionesP();
        // Graficar
        prioridad();
        // Tabla
        PorPrioridades();
        cout<<"POR PRIORIDADES"<<endl;

        ui->tableWidget->setRowCount(0);   //resetea tabla
        int fin = 0;
        int retorno = 0;
        double normalizado = 0;
        F=0;

        for(auto i:Pr)
        {
            proc = std::get<0>(i);
            lleg = std::get<1>(i);
            serv = std::get<2>(i);
            entr = std::get<3>(i);
            esp = std::get<4>(i);
            fin = std::get<5>(i);
            retorno = std::get<6>(i);
            normalizado = std::get<7>(i);
            prior = std::get<8>(i);

            ui->tableWidget->insertRow(F);
            ui->tableWidget->setItem(F,0, new QTableWidgetItem(QString::fromStdString(proc)));
            ui->tableWidget->setItem(F,1, new QTableWidgetItem(QString::number(lleg)));
            ui->tableWidget->setItem(F,2, new QTableWidgetItem(QString::number(serv)));
            ui->tableWidget->setItem(F,3, new QTableWidgetItem(QString::number(fin)));
            ui->tableWidget->setItem(F,4, new QTableWidgetItem(QString::number(retorno)));
            ui->tableWidget->setItem(F,5, new QTableWidgetItem(QString::number(normalizado)));
            ui->tableWidget->setItem(F,6, new QTableWidgetItem(QString::number(entr)));
            ui->tableWidget->setItem(F,7, new QTableWidgetItem(QString::number(esp)));
            ui->tableWidget->setItem(F,8, new QTableWidgetItem(QString::number(prior)));
            F++;
        }

    }

    else
        std::cout<<"NO hay :v"<<std::endl;

    std::cout<< "Tiempor de retorno promedio: "<<R_promedio<<std::endl;
    std::cout<< "Tiempo de retorno normalizado promedio: "<<RN_promedio<<std::endl;

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


void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    if(index==0){
        std::cout<<"FIFO seleccionada"<<std::endl;
    }
    else if(index==1){
        std::cout<<"Mas corto primero seleccionado"<<std::endl;
    }
    else if(index==2){
        std::cout << "Shortest Remaining Time seleccionado"<<std::endl;
    }
    else if(index==3){
        cout<<"ROUND ROBIN seleccionado"<<endl;
    }
    else if(index==4){
        cout<<"Por prioridad seleccionado"<<endl;
    }
}
