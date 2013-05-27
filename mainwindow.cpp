#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QColor>

double MainWindow::sigma = 1.4;
const short MainWindow::k_size = 3;
const double MainWindow::PI = qAtan(1)*4;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupGraphicalInterface(false);
    insertBlankLinesInTerminal();

    qDebug() << "The kernel will be constructed as follows...";
    testKernelCoordinateScheme(k_size);

    qDebug() << "Initializing kernel...";
    kernel = manual3x3Kernel();
    printMatrix(kernel, k_size);

    loadImage();

    QVector<QRgb> modifiedImg = convolveImage(image, kernel, k_size);
    qimageFromPixelVector(modifiedImg, image->width(),
				  image->height(), image->format());

    QString savepath = "/home/eric/Dropbox/cells/images/output.jpg";
    image->save(savepath);
    displayImage(savepath);
}

QVector<QRgb>
MainWindow::convolveImage(QImage* img, QVector<double> kernel, short kernel_size)
{
    int width = img->width();
    int height = img->height();
    int len = width * height;

    QVector<QRgb> imvec = vectorOfImagePixels(img);
    QVector<QRgb> retvec(0);

    qDebug() << "Length of image vector:" << len;

    for(int i = 0; i < len; ++i) // perform the convolution
    {
	retvec.append(convolveImageAtPixel(imvec, width,
					   kernel, kernel_size,
					   i));
    }

    return retvec;
}

QRgb
MainWindow::convolveImageAtPixel(QVector<QRgb> img, short img_width,
				 QVector<double> kernel, short kernel_size,
				 int center)
{
    short y, x;
    QRgb sum = 0;
    QString out = "";

    for(int ky = 0; ky < kernel_size; ++ky)
    {
	y = center/img_width - kernel_size/2 + ky;
	int kColFromY = kernel_size * ky;
	int imgColFromY = img_width * y; // only calculate when new

	for(int kx = 0; kx < kernel_size; ++kx)
	{
	    x = center%img_width - kernel_size/2 + kx;

	    if (x < 0 || y < 0 || x >= img_width)
		continue;

	    double kval = kernel[kColFromY + kx];
	    sum += img[imgColFromY + x] * kval;
	}
    }

    return sum;
}


void
MainWindow::qimageFromPixelVector(QVector<QRgb> imgvec, short width,
				  short height, QImage::Format format)
{
    (void)format;
    int len = width * height;
    QImage* img = image;

    for(int i = 0; i < len; ++i)
    {
	img->setPixel(i%width, i/width, imgvec[i]);
    }
}

QVector<QRgb>
MainWindow::vectorOfImagePixels(QImage* img)
{
    QVector<QRgb> vec(0);
    for(int y = 0; y < img->height(); ++y)
	for(int x = 0; x < img->width(); ++x)
	    vec.append(img->pixel(x, y));
    return vec;
}

QVector<double>
MainWindow::manual5x5Kernel()
{
    QVector<double> k(0);
    k.append(0); k.append(0); k.append(1);   k.append(0); k.append(0);
    k.append(0); k.append(1); k.append(2);   k.append(1); k.append(0);
    k.append(1); k.append(2); k.append(-16); k.append(2); k.append(1);
    k.append(0); k.append(1); k.append(2);   k.append(1); k.append(0);
    k.append(0); k.append(0); k.append(1);   k.append(0); k.append(0);
    return k;
}

QVector<double>
MainWindow::manual3x3Kernel()
{
    QVector<double> k(0);
    k.append(-1); k.append(-1); k.append(-1);
    k.append(-1); k.append(8); k.append(-1);
    k.append(-1); k.append(-1); k.append(-1);
    return k;
}

QVector<double>
MainWindow::normalizeMatrix(QVector<double> k, short size)
{
    int len = qPow(size, 2);
    double max = 0;		// First, obtain the max
    for(short i = 0; i < len; ++i) // of the data set
	if(qAbs(k[i]) > max)
	    max = qAbs(k[i]);

    for(short i = 0; i < len; ++i) // Then, adjust each term
	k[i] = k[i]/max;

    return k;
}

double
MainWindow::maxOfMatrix(QVector<double> k, short size)
{
    double max = 0;
    for(short i = 0; i < qPow(size, 2); ++i)
	if(qAbs(k[i]) > max)
	    max = qAbs(k[i]);

    return max;
}

void
MainWindow::printStatsAboutMatrix(QVector<double> k, short size)
{
    double neg = negOfMatrix(k, size);
    double pos = posOfMatrix(k, size);
    double max = maxOfMatrix(k, size);
    double sum = sumOfMatrix(k, size);
    qDebug() << "Sum of matrix:\t" << sum
	     << "\nMaximum value:\t" << max
	     << "\nNegative values:" << neg
	     << "\nPositive values:" << pos
	     << "\nratio:\t\t" << -neg/pos << "\n";
}

double
MainWindow::sumOfMatrix(QVector<double> k, short size)
{
    double sum = 0;
    for(short i = 0; i < qPow(size, 2); ++i)
	sum += k[i];
    return sum;
}

QVector<double>
MainWindow::generateKernel(short size) //size on one side
{
    int len = qPow(size, 2);
    QVector<double> k(len);
    for(short i = 0; i < len; ++i)
    	k[i] = laplacianOfGaussian(size, i);

    return k;
}

double
MainWindow::laplacianOfGaussian(short size, short i)
{
    // short x = i%size, y = i/size;
    short x = qAbs(i%size - size/2);
    short y = qAbs(i/size - size/2);
    // double frac = -((double) (qPow(x,2) + qPow(y,2))) /
    // 	((double) (2*qPow(MainWindow::sigma,2)));
    // double tmp = -1/(MainWindow::PI*qPow(MainWindow::sigma, 4)) *
    // 	(1+frac) * qExp(frac);
    double tmp = (qPow(x,2) + qPow(y,2) - 2*qPow(MainWindow::sigma,2))
	/ qPow(sigma,4) * qExp(-(qPow(x,2)+qPow(y,2)
				 / 2*qPow(MainWindow::sigma,2)));
    return tmp;
}

void
MainWindow::testKernelCoordinateScheme(short size)
{
    int len = qPow(size, 2);
    QString out = "";
    QVector<double> k(len);

    for(short j = 0; j < size; ++j)
    {
	for(short i = 0; i < size; ++i)
	{
	    int idx = size*j + i;
	    out.append(QString::number(qAbs(idx%size-size/2)))
		.append(",")
		.append(QString::number(qAbs(idx/size - size/2)))
		.append("\t");
	}
	qDebug() << out.trimmed();
	out.clear();
    }
    qDebug() << "";
}

double
MainWindow::posOfMatrix(QVector<double> k, short size)
{
    double sum = 0;
    for(short i = 0; i < qPow(size, 2); ++i)
	if(k[i] > 0)
	    sum += k[i];

    return sum;
}

double
MainWindow::negOfMatrix(QVector<double> k, short size)
{
    double sum = 0;
    for(short i = 0; i < qPow(size, 2); ++i)
	if(k[i] < 0)
	    sum += k[i];

    return sum;
}

void
MainWindow::printMatrix(QVector<double> k, short size)
{
    QString line = "";
    for(short j = 0; j < size; ++j) // print each row
    {
	for(short i = 0; i < size; ++i) // print column
	{
	    line.append(QString::number(k[i*size + j] //, 'g', 1
			    ) .append("\t"));
	}

	qDebug() << line.trimmed();
	line.clear();
    }
    qDebug() << "";
    printStatsAboutMatrix(k, size);
}

void
MainWindow::displayImage(QString imgPath)
{
    displayPane->setPixmap( QPixmap(imgPath) );
}

void
MainWindow::loadImage()
{
    imagePath = "/home/eric/Dropbox/cells/images/first.jpg";

    displayImage(imagePath);
    image->load(imagePath);
    qDebug() << "Image dimensions:" << image->width()
	     << "x" << image->height();
}

void
MainWindow::setupGraphicalInterface(bool enableButtons)
{
    mainLayout = new QHBoxLayout;

    imageControlBox = new QVBoxLayout;
    mainLayout->addLayout(imageControlBox);

    image = new QImage();
    displayPane = new QLabel();
    displayBox =  new QVBoxLayout;
    displayBox->addWidget(displayPane);
    mainLayout->addLayout(displayBox);

    loadBitMapButton = new QPushButton("Load &bmp", this);
    loadBitMapButton->setEnabled(enableButtons);
    connect(loadBitMapButton, SIGNAL(clicked()),
	    this, SLOT(loadImage()));
    imageControlBox->addWidget(loadBitMapButton);

    displayBitMapButton = new QPushButton("Display bmp", this);
    displayBitMapButton->setEnabled(enableButtons);
    connect(displayBitMapButton, SIGNAL(clicked()),
	    this, SLOT(loadImage()));
    imageControlBox->addWidget(displayBitMapButton);

    ui->centralWidget->setLayout(mainLayout);

}

MainWindow::~MainWindow()
{
    delete ui;
}
