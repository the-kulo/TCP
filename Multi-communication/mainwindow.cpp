#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    TCP_Server = new QTcpServer();
    if (TCP_Server->listen(QHostAddress::Any,8080))
    {
        connect(TCP_Server,SIGNAL(newConnection()),this,SLOT(newConnection()));
        QMessageBox::information(this,"Qt With Ketan", "Server Start.");
    }
    else
    {
        QMessageBox::information(this,"Qt With Ketan", "Server Start Fail.");
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::newConnection()
{
    while (TCP_Server->hasPendingConnections())
    {
        Add_New_Client_Connection(TCP_Server->nextPendingConnection());
    }
}

void MainWindow::Read_Data_From_Socket()
{

}

void MainWindow::Add_New_Client_Connection(QTcpSocket *socket)
{
    Client_Connection_List.append(socket);
    connect(socket,SIGNAL(readyRead()),this,SLOT(Ready_Data_From_Socket()));
    ui->comboBox_Client_List->addItem(QString::number(socket->socketDescriptor()));
    QString Client = "Client : " + QString::number(socket->socketDescriptor()) + " Connected With The Server.";
    ui->textEdit_Client_Messages->append(Client);
}

void MainWindow::on_pushButton_Send_clicked()
{
    QString Message_For_Client = ui->lineEdit_Message_For_Client->text();
    QString Receiver = ui->comboBox_Client_List->currentText();

    if (ui->comboBox_Send_Message_Type->currentText() == "All")
    {
        foreach (QTcpSocket *socket, Client_Connection_List)
        {
            socket->write(Message_For_Client.toStdString().c_str());
        }
    }
    else
    {
        foreach (QTcpSocket *socket, Client_Connection_List)
        {
            if (socket->socketDescriptor() == Receiver.toLongLong())
            {
                socket->write(Message_For_Client.toStdString().c_str());
            }
        }

    }
}

