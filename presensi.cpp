#include "presensi.h"
#include "ui_presensi.h"

#include <QFont>
#include <QTimer>
#include <QTime>
#include <stdio.h>
#include <stdlib.h>
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/legacy/legacy.hpp"
#include <zbar.h>
#include "zbar/Image.h"
#include "zbar/ImageScanner.h"
#include "zbar/Video.h"
#include <zint.h>

using namespace zbar;
using namespace cv;

typedef struct parameter Parameter;
struct parameter {
    float alpha;
    float beta;
    float gamma;
};

int thresh = 100;
int max_thresh = 255;
RNG rng(12345);
Mat src_gray;

struct zint_symbol *my_symbol;

CvCapture* capture;
QTimer *timer;
int symbology = 1;

void thresh_callback(int, void*);

Presensi::Presensi(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Presensi)
{
    ui->setupUi(this);
//    QFont f("Arial", 28, QFont::Bold);
//    ui->labelJam->setFont(f);

//    QTimer * jam = new QTimer(this);

//    QObject::connect(timer,SIGNAL(timeout()),this,SLOT(tampilJam()));

//    timer->start(1000);
    this->tampilJam();

    addListSymbology();
}

Presensi::~Presensi()
{
    cvReleaseCapture( &capture );
    delete ui;
}

void Presensi::on_actionAbout_triggered()
{
    qApp->aboutQt();
}

void Presensi::on_btnOke_clicked()
{
    //Kiem tra trang thai bat tat cua camera
    if(capture == 0)
    {
        capture = cvCaptureFromCAM(CV_CAP_ANY);
        if(capture)
        {
            cvSetCaptureProperty(capture,CV_CAP_PROP_FRAME_HEIGHT,1600);
            cvSetCaptureProperty(capture,CV_CAP_PROP_FRAME_WIDTH,1600);

            timer = new QTimer(this);

            QObject::connect(timer,SIGNAL(timeout()),this,SLOT(ProcessFrame()));
            QObject::connect(timer,SIGNAL(timeout()),this,SLOT(tampilJam()));

            timer->start(1000/15);
            this->tampilJam();
        } else {
            ui->lineNik->setText("Cannot connect to Camera!");
        }
    }

    return ;
}

void Presensi::ProcessFrame()
{
    IplImage* src = cvQueryFrame(capture);
    QImage img_show = QImage((unsigned char*)(src->imageData),src->width,src->height,QImage::Format_RGB888).rgbSwapped();
    ui->areaBarcode->setPixmap(QPixmap::fromImage(img_show,Qt::AutoColor).scaledToWidth(300));

    int n = ScanIDCard(src);

    if(n)
    {
        timer->stop();
        cvReleaseCapture(&capture);
    }
}

int Presensi::ScanIDCard(IplImage *src)
{
    IplImage *img = cvCreateImage(cvSize(src->width,src->height), IPL_DEPTH_8U,1);

    cvCvtColor( src, img, CV_RGB2GRAY );

    //Su dung thu vien Zbar de giai ma

    uchar* raw = (uchar *)img->imageData;
    int width = img->width;
    int height = img->height;

    ImageScanner *scanner=new ImageScanner();

    scanner->set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);

    Image *zimg=new Image(width,height,"Y800",raw,width*height);

    int n=scanner->scan(*zimg);

    if(n>0)
    {
        for(Image::SymbolIterator symbol = zimg->symbol_begin(); symbol != zimg->symbol_end(); ++symbol)
        {
            //Hien thi ket qua len Textbox
//            ui->lineNik->setText(QString::fromStdString(symbol->get_data()));
            this->tampilkanHasil(QString::fromStdString(symbol->get_data()));
        }

    }

    else
    {
//        ui->lineNik->setText("Cannot Detect Code!");
        this->tampilkanHasil("CHIGAU_DETA");
    }

    //Giai phong tai nguyen
    zimg->set_data(NULL, 0);

    return n;
}

void thresh_callback(int, void*)
{

    //Mat src_copy = src.clone();
    Mat threshold_output;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    /// Detect edges using Threshold
    threshold( src_gray, threshold_output, thresh, 255, THRESH_BINARY );
    /// Find contours
    findContours( threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
    /// Find the convex hull object for each contour
    vector<vector<Point> >hull( contours.size() );
    for( int i = 0; i < contours.size(); i++ )
    { convexHull( Mat(contours[i]), hull[i], false ); }
    /// Draw contours + hull results
    Mat drawing = Mat::zeros( threshold_output.size(), CV_8UC3 );
    for( int i = 0; i< contours.size(); i++ )
    {
    Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
    drawContours( drawing, contours, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
    drawContours( drawing, hull, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
    }
    /// Show in a window
    namedWindow( "Hull demo", CV_WINDOW_NORMAL);
    imshow( "Hull demo", drawing );

}

void Presensi::use_drawRectangle()
{
      IplImage *src_img = 0, *dst_img;
      IplImage *src_img_gray = 0;
      IplImage *tmp_img;
      CvMemStorage *storage = cvCreateMemStorage (0);
      CvSeq *contours = 0;
      CvBox2D ellipse;
      CvTreeNodeIterator it;
      CvPoint2D32f pt[4];

      IplImage* src = cvQueryFrame(capture);


      src_img=src;

      src_img_gray = cvCreateImage (cvGetSize (src_img), IPL_DEPTH_8U, 1);
      cvCvtColor (src_img, src_img_gray, CV_BGR2GRAY);
      tmp_img = cvCreateImage (cvGetSize (src_img), IPL_DEPTH_8U, 1);
      dst_img = cvCloneImage (src_img);

      cvThreshold (src_img_gray, tmp_img, 100, 255, CV_THRESH_BINARY);
      cvFindContours (tmp_img, storage, &contours, sizeof (CvContour),
                      CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cvPoint (0, 0));

      cvInitTreeNodeIterator (&it, contours, 3);

      while ((contours = (CvSeq *) cvNextTreeNode (&it)) != NULL)
      {
        if (contours->total > 5) {

          ellipse = cvFitEllipse2 (contours);
          ellipse.angle = 90.0 - ellipse.angle;

          cvDrawContours (dst_img, contours, CV_RGB (255, 0, 0), CV_RGB (255, 0, 0), 0, 1, CV_AA, cvPoint (0, 0));
          //cvEllipseBox (dst_img, ellipse, CV_RGB (0, 0, 255), 2);
          //cvBoxPoints (ellipse, pt);
          cvLine (dst_img, cvPointFrom32f (pt[0]), cvPointFrom32f (pt[1]), CV_RGB (0, 255, 0));
          cvLine (dst_img, cvPointFrom32f (pt[1]), cvPointFrom32f (pt[2]), CV_RGB (0, 255, 0));
          cvLine (dst_img, cvPointFrom32f (pt[2]), cvPointFrom32f (pt[3]), CV_RGB (0, 255, 0));
          cvLine (dst_img, cvPointFrom32f (pt[3]), cvPointFrom32f (pt[0]), CV_RGB (0, 255, 0));
        }

      }

      //cvNamedWindow ("Fitting", CV_WINDOW_NORMAL);
      //cvShowImage ("Fitting", dst_img);

      QImage img_show = QImage((unsigned char*)(dst_img->imageData),dst_img->width,dst_img->height,QImage::Format_RGB888).rgbSwapped();
      ui->areaBarcode->setPixmap(QPixmap::fromImage(img_show,Qt::AutoColor).scaledToWidth(300));
      int n = ScanIDCard(dst_img);

      if(n)
      {
          timer->stop();
          cvReleaseCapture(&capture);
      }

      /*
      cvWaitKey (0);
      cvDestroyWindow ("Fitting");
      cvReleaseImage (&src_img);
      cvReleaseImage (&dst_img);
      cvReleaseImage (&src_img_gray);
      cvReleaseImage (&tmp_img);*/
      cvReleaseMemStorage (&storage);

}

void Presensi::tampilkanHasil(QString kode)
{
    //
    if(kode != "CHIGAU_DETA") {
        ui->lineNik->setText(kode);
        ui->lineNama->setText("DUMMY");
        ui->lineJabatan->setText("DUMMY");

        // set icon green, dummy
        ui->iconStatus->setText("O");
    }
}

void Presensi::showImageEncode()
{
    //
}

void Presensi::readFromFileText()
{
    //
}

void Presensi::addListSymbology()
{
    // tidak penting banget
}

void Presensi::enableQrCode()
{
    //
}

void Presensi::disableBarcode()
{
    //
}

void Presensi::clearAll()
{
    //
}

void Presensi::tampilJam()
{
    QTime waktu = QTime::currentTime();
    QString jam = waktu.toString("hh.mm");
    ui->labelJam->setText(jam);
}

void Presensi::on_actionSettings_triggered()
{
    //
}
