#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    TCP_Socket = new QTcpSocket();

    connect(TCP_Socket, &QTcpSocket::readyRead, this, &MainWindow::readSocket);
    connect(TCP_Socket, &QTcpSocket::disconnected, this, &MainWindow::discardSocket);

    QString IPAddress = "192.168.164.70";
    TCP_Socket->connectToHost(QHostAddress(IPAddress), 43000);
    if (TCP_Socket->waitForConnected(30000))
    {
        ui->statusbar->showMessage("Socket (Client) is connected");
    }
    else
    {
        ui->statusbar->showMessage("Socket (Client) is  not connected:" + TCP_Socket->errorString());
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::readSocket()
{
    QByteArray DataBuffer;

    QDataStream socketstream(TCP_Socket);
    socketstream.setVersion(QDataStream::Qt_6_2);

    socketstream.startTransaction();
    socketstream >>DataBuffer;

    if (socketstream.commitTransaction() == false)
    {
        return;
    }

    QString HeaderData = DataBuffer.mid(0, 128);

    QString Filename = HeaderData.split(",")[0].split(":")[1];
    QString FileExt = Filename.split(".")[1];
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
    TCP_Socket->deleteLater();
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

void MainWindow::on_pushButton_Send_File_clicked()
{
    if (TCP_Socket)
    {
        if (TCP_Socket->isOpen())
        {
            QString FilePath = QFileDialog::getOpenFileName(this,"Select File", QCoreApplication::applicationDirPath(),"File(*.jpg *.txt *.png *.bmp)");

            Send_File(TCP_Socket, FilePath);
        }
    }
}

