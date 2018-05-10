#include <QApplication>
#include "myview.h"
#include <QTextCodec>
#include <QTime>

int main(int argc,char *argv[])
{
    QApplication app(argc,argv);
    QTextCodec::setCodecForLocale(QTextCodec::codecForLocale());
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    MyView view;
    view.show();
    return app.exec();
}
