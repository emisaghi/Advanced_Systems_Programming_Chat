#ifndef WIDGET_H
#define WIDGET_H

#include <stdlib.h>
#include <QWidget>
#include <QApplication>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QImage>
#include <QPixmap>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHostInfo>

class QAction;

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private:
    QTextEdit *maint;
    QTextEdit *tempt;
    QPushButton *msgsend;
    QPushButton *imgsend;
    QPushButton *browse;
    QPushButton *about;
    QPushButton *quit;
    QLabel *imglabel;
    QLabel *msglabel;
    QLabel *sip;
    QLabel *selimg;
    QLabel *imginfo;
    QLineEdit *ipl;
    QLineEdit *namel;
    QHBoxLayout *mainb;
    QVBoxLayout *vbox1;
    QVBoxLayout *vbox2;

    bool firsttime;
    QString message_string;

    QByteArray *imgba;
    QTcpServer tcpServer;
    QTcpSocket tcpClient;
    QTcpSocket *tcpServerConnection;

    QUdpSocket  *udpSocket;
    QUdpSocket  *udpSocket_received;

    QPixmap  *pix;
    QPixmap *selpix;
    QImage  *img;


    QHostAddress senderAddress;


public:
    void server_listen();

public slots:
    void send_image();
    void browse_image();

    void display_about();

    void acceptConnection();
    void startTransfer();
    void receive();
    void displayError(QAbstractSocket::SocketError socketError);

    void udpsend();
    void udpreceive();

private:
    Ui::Widget *ui;
};

#endif // WIDGET_H
