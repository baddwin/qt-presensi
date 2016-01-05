#ifndef PRESENSI_H
#define PRESENSI_H

#include <QMainWindow>
#include "opencv/cv.h"
#include "opencv/highgui.h"

namespace Ui {
class Presensi;
}

class Presensi : public QMainWindow
{
    Q_OBJECT

public:
    explicit Presensi(QWidget *parent = 0);
    ~Presensi();

private slots:
    void on_actionAbout_triggered();

    void on_btnOke_clicked();
    void addListSymbology();
    void ProcessFrame();
    void showImageEncode();
    void readFromFileText();
    void use_drawRectangle();
    void snake();
    void clearAll();
    void disableBarcode();
    void enableQrCode();
    void tampilJam();

private:
    Ui::Presensi *ui;
    int ScanIDCard(IplImage *src);
};

#endif // PRESENSI_H
