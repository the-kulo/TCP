#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork>
#include <QtGui>
#include <QtCore>
#include <QtWidgets>

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
    void readSocket();
    void discardSocket();

    void Send_File(QTcpSocket *socket, QString filename);

    void on_pushButton_Send_File_clicked();

private:
    Ui::MainWindow *ui;
    QTcpSocket *TCP_Socket;
};
#endif // MAINWINDOW2_H
