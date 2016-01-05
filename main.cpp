#include "presensi.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Presensi w;
    w.show();

    return a.exec();
}
