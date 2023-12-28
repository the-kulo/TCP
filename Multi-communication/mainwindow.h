#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QList>
#include <QByteArray>
#include <QMessageBox>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void newConnection();
    void Read_Data_From_Socket();

    void on_pushButton_Send_clicked();

private:
    void Add_New_Client_Connection(QTcpSocket *socket);

private:
    Ui::MainWindow *ui;
    QTcpServer *TCP_Server;
    QList<QTcpSocket*> Client_Connection_List;
};
#endif // MAINWINDOW_H
