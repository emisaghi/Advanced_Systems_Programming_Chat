#include "widget.h"
#include "ui_widget.h"
#include <QtWidgets>


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    QVBoxLayout *vbox1=new QVBoxLayout;
    QVBoxLayout *vbox2=new QVBoxLayout;
    QHBoxLayout *mainb=new QHBoxLayout;
    maint=new QTextEdit;
    tempt=new QTextEdit;
    imglabel=new QLabel;
    msglabel=new QLabel;
    imginfo=new QLabel;
    selimg=new QLabel;
    ipl=new QLineEdit;
    namel=new QLineEdit;
    sip=new QLabel;
    QPushButton *imgsend=new QPushButton("Send Image");
    QPushButton *msgsend=new QPushButton("Send Message");
    QPushButton *browse=new QPushButton("Browse Image");
    QPushButton *about=new QPushButton("About");
    QPushButton *quit=new QPushButton("Quit");
    imglabel->setText("Received image will be shown here.");
    ipl->setText("Enter IP");
    namel->setText("Enter Name");
    msglabel->setText("Message");
    sip->setText("Sender IP will be shown here.");
    selimg->setText("Selected image will be shown here.");

    maint->setReadOnly(true);
    vbox1->addWidget(imglabel);
    vbox1->addWidget(ipl);
    vbox1->addWidget(namel);
    vbox1->addWidget(sip);
    vbox1->addWidget(imginfo);
    vbox1->addWidget(selimg);
    vbox1->addWidget(browse);
    vbox1->addWidget(imgsend);
    vbox1->addWidget(about);
    vbox1->addWidget(quit);
    vbox2->addWidget(maint);
    vbox2->addWidget(msglabel);
    vbox2->addWidget(tempt);
    vbox2->addWidget(msgsend);
    mainb->addLayout(vbox2);
    mainb->addLayout(vbox1);
    setLayout(mainb); //two vertical boxes are inserted into a horizontal box, which is the main widget of the program.

    pix=new QPixmap();
    selpix=new QPixmap();

    imgba= new QByteArray;

    udpSocket_received= new QUdpSocket(this);
    udpSocket_received->bind(QHostAddress::Any,3030,QAbstractSocket::ShareAddress);

    connect(msgsend, SIGNAL(clicked()),this, SLOT(udpsend()));
    connect(imgsend , SIGNAL(clicked()) , this, SLOT(send_image()));
    connect(about, SIGNAL(clicked()), this, SLOT(display_about()));
    connect(quit, SIGNAL(clicked()), this, SLOT(close()));
    connect(browse, SIGNAL(clicked()), this, SLOT(browse_image()));

    connect(&tcpServer  , SIGNAL(newConnection()),this, SLOT(acceptConnection()));
    connect(&tcpClient, SIGNAL(connected()), this, SLOT(startTransfer()));
    connect(&tcpClient, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(displayError(QAbstractSocket::SocketError)));
    connect(udpSocket_received,SIGNAL(readyRead()),this,SLOT(udpreceive()));

    QString nameString=QString(getenv("USERNAME"))+'@'+QHostInfo::localHostName();
    //by default, the user and pc names are used for chatting, which can be changed later on.
    namel->setText(nameString);

    bool ok;
    QString namestr=QInputDialog::getText(this, "Name", "Enter your name please!",
                                          QLineEdit::Normal, nameString, &ok);
    //an input dialog is used to enter the name before starting to chat.
    if (ok==true)
    namel->setText(namestr);

    server_listen();

    //this is to show the message "... has joined the chat!" just when the person joines for the first time.
    firsttime=true;
    udpsend();
}

void Widget::server_listen()
{
    //this is to listen and throw an error if something wrong happens.
    while (!tcpServer.isListening() && !tcpServer.listen(QHostAddress::Any,45454)) {
        QMessageBox::StandardButton ret = QMessageBox::critical(this,tr("Chat Program"),
                                      tr("The following error has occured!\n%1")
                                                          .arg(tcpServer.errorString()),
                                        QMessageBox::Retry | QMessageBox::Cancel);
        if (ret == QMessageBox::Cancel)
            return;
    }
}

void Widget::browse_image()
{
    //browsing the image file using a file dialog.
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                     "",
                                                     tr("Files (*.*)"));
    img=new QImage(fileName);
    imginfo->setText("The selected image is:");
    selpix->load(fileName);
    selimg->setPixmap(*selpix);
}

void Widget::send_image()
{
    QHostAddress destination_ip(ipl->text());
    tcpClient.open(QIODevice::ReadWrite);
    tcpClient.connectToHost(destination_ip, 45454);
}

void Widget::acceptConnection()
{
    imgba->clear();
    tcpServerConnection = tcpServer.nextPendingConnection();
    connect(tcpServerConnection, SIGNAL(readyRead()),this, SLOT(receive()));
    connect(tcpServerConnection, SIGNAL(error(QAbstractSocket::SocketError))
            ,this, SLOT(displayError(QAbstractSocket::SocketError)));
}


void Widget::startTransfer()
{
    QByteArray arr;
    QBuffer buff(&arr);
    buff.open(QIODevice::WriteOnly);
    img->save(&buff, "PNG");
    tcpClient.write(arr);
    tcpClient.close();
}


void Widget::receive()
{
    imgba->append(tcpServerConnection->readAll());
    pix->loadFromData(*imgba,"PNG");
    imglabel->setPixmap(*pix);
    sip->setText("Sender IP:\n"+tcpServerConnection->peerAddress().toString());
}

void Widget::udpsend()
{
//for the first time, the message "... has joined chat!" is sent; otherwise the message in the textbox is sent.
    if (firsttime==true)
    {
        message_string = "<"+namel->text()+"> has joined the chat!";

    }
    else
        message_string = '[' + namel->text() + "]: "+ tempt->toPlainText();
//if it is the first time or the textbox is not empty, data is sent.
    if(tempt->toPlainText()!="" || firsttime==true)
    {
    udpSocket = new QUdpSocket(this);
    QByteArray datagram;
    datagram.append(message_string);
    udpSocket->writeDatagram(datagram.data(), datagram.size(),QHostAddress::Broadcast, 3030);
    tempt->setText("");
    firsttime=false;
    }
}
void Widget::udpreceive()
{
    //The messages and the senders IP address is received in this function.
    while(udpSocket_received->hasPendingDatagrams())
    {
        QByteArray received_datagram;
        received_datagram.resize(udpSocket_received->pendingDatagramSize());
        udpSocket_received->readDatagram(received_datagram.data(),received_datagram.size(),
                                         &senderAddress);
        maint->append("["+senderAddress.toString()+"]-"+received_datagram.data());
    }
}

void Widget::displayError(QAbstractSocket::SocketError socketError)
{
    //Errors are displayed with this function.
    if (socketError == QTcpSocket::RemoteHostClosedError)
        return;

    QMessageBox::information(this, tr("Network error"),
                             tr("The following error occurred: %1.")
                             .arg(tcpClient.errorString()));
    tcpClient.close();
    tcpServer.close();
}

void Widget::display_about()
{
    QMessageBox::about(this, "About",
                       "This chat program is written by Ehsan Misaghi and Alireza Sayyah!");
}

Widget::~Widget()
{
    delete ui;
}
