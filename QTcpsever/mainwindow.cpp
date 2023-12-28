#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    Tcp_Server = new QTcpServer();
    if (Tcp_Server->listen(QHostAddress::AnyIPv4,43000))
    {
        connect(Tcp_Server, &QTcpServer::newConnection, this, &MainWindow::newConnection);
        ui->statusbar->showMessage("TCP Server Started");
    }
    else
    {
        QMessageBox::information(this, "TCP Server Error", Tcp_Server->errorString());
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::readSocket()
{
    QTcpSocket *socket = reinterpret_cast<QTcpSocket*>(sender());

    QByteArray DataBuffer;

    QDataStream socketstream(socket);
    socketstream.setVersion(QDataStream::Qt_6_2);

    socketstream.startTransaction();
    socketstream >>DataBuffer;

    if (socketstream.commitTransaction() == false)
    {
        return;
    }

    QString HeaderData = DataBuffer.mid(0, 128);

    QString Filename = HeaderData.split(",")[0].split(":")[1];
    QString FileExt = Filename.split("；")[1];
    QString FileSize = HeaderData.split(",")[1].split(":")[1];

    DataBuffer = DataBuffer.mid(128);

    QString SaveFilePath = QCoreApplication::applicationDirPath() + "/" + Filename;

    QFile File(SaveFilePath);
    if (File.open(QIODevice::WriteOnly))
    {
        File.write(DataBuffer);
        File.close();
    }
}

void MainWindow::discardSocket()
{
    QTcpSocket *socket = reinterpret_cast<QTcpSocket*>(sender());
    int idx = Client_List.indexOf(socket);
    if (idx > -1)
    {
        Client_List.removeAt(idx);
    }

    ui->comboBox_Client_List->clear();
    foreach (QTcpSocket *sockettemp, Client_List)
    {
        ui->comboBox_Client_List->addItem(QString::number(sockettemp->socketDescriptor()));
    }

    socket->deleteLater();
}

void MainWindow::newConnection()
{
    while (Tcp_Server->hasPendingConnections())
    {
        AddToSocketList(Tcp_Server->nextPendingConnection());
    }
}

void MainWindow::AddToSocketList(QTcpSocket *socket)
{
    Client_List.append(socket);
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::readSocket);
    connect(socket, &QTcpSocket::disconnected, this, &MainWindow::discardSocket);
    ui->textEdit_Messages->append("Client is connected with server : Socket ID : " + QString::number(socket->socketDescriptor()));
    ui->comboBox_Client_List->addItem(QString::number(socket->socketDescriptor()));
}

void MainWindow::on_pushButton_Send_File_clicked()
{
    QString FilePath = QFileDialog::getOpenFileName(this,"Select File", QCoreApplication::applicationDirPath(),"File(*.jpg *.txt *.png *.bmp)");

    if (ui->comboBox_Transfer_Type->currentText() == "Broadcast")
    {
        foreach (QTcpSocket *sockettemp, Client_List)
        {
            Send_File(sockettemp, FilePath);
        }
    }
    else if (ui->comboBox_Transfer_Type->currentText() =="receiver")
    {
        QString receiverid = ui->comboBox_Client_List->currentText();
        foreach (QTcpSocket *sockettemp, Client_List)
        {
            if (sockettemp->socketDescriptor() ==receiverid.toLongLong())
            {
                Send_File(sockettemp, FilePath);
            }
        }
    }
}

void MainWindow::Send_File(QTcpSocket *socket, QString filename)
{
    if (socket)
    {
        if (socket->isOpen())
        {
            QFile filedata(filename);
            if (filedata.open(QIODevice::ReadOnly))
            {
                QFileInfo fileinfo(filedata);
                QString FileNameWithExt(fileinfo.fileName());

                QDataStream socketstream(socket);
                socketstream.setVersion(QDataStream::Qt_6_2);

                QByteArray header;
                header.prepend("filename:" + FileNameWithExt.toUtf8() + ",filesize:" + QString::number(filedata.size()).toUtf8());
                header.resize(128);

                QByteArray ByteFileData = filedata.readAll();
                ByteFileData.prepend(header);

                socketstream << ByteFileData;
            }
            else
            {
                qDebug() << "File not open";
            }
        }
        else
        {
            qDebug() << "Client Socket not open";
        }
    }
    else
    {
        qDebug() << "Client Socket not invalid";
    }
}
