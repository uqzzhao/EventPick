#include "mer.h"
#include <QApplication>
#include <QTextCodec>

#include <QDesktopWidget>
#include <QPixmap>
#include <QSplashScreen>
#include <QElapsedTimer>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    //设置程序启动画面
    QPixmap pixmap(":/resources/start.png");
    QSplashScreen splash(pixmap);
    splash.show();

    // 设置画面停留时间为3秒
    QElapsedTimer timer;
    timer.start();
    while (timer.elapsed()<3000)
    {
        QCoreApplication::processEvents();
    }




    Mer mer;
    mer.setWindowTitle("EventPick");


    mer.show();
    mer.move ((QApplication::desktop()->width() - mer.width())/2,
             (QApplication::desktop()->height() - mer.height())/2);

    splash.finish(&mer);

    return a.exec();


}
