//Alireza Sayyah (8923010) and Ehsan Misaghi (8923706)
//Chat Project
//Advanced System Programming
//Electrical Engineering Faculty, Amirkabir University of Technology

#include "widget.h"
#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    //Window icon and title is set here.
    w.setWindowTitle("Chat Program");
    w.setWindowIcon(QIcon("Chat.ico"));

    w.show();
    return a.exec();
}
