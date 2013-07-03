// An application to spatially process images.
// Copyright (C) 2013 Eric Crosson
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QtCore/qmath.h>
#include <QPixmap>
#include <QString>
#include <QVector>
#include <QImage>
#include <QLabel>
#include <QDebug>

#define QRgb unsigned int

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /* Qt defined */
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    /* Use this function to load an image into the QImage */
    void loadImage();

    /* Use this function to display the image in its current state */
    void displayImage(QString imgPath = "");

private:
    /* private methods */
    void setupGraphicalInterface(bool enableButtons);
    void insertBlankLinesInTerminal() {qDebug() << "\n\n\n";}
    void testKernelCoordinateScheme(short size);
    void printMatrix(QVector<double> k, short size);
    void printStatsAboutMatrix(QVector<double> k, short size);

    /* These functions return a modified matrix
     * Caution- they do not generate a matrix whose sum of
     *          numbers is equal to one.
     */
    QVector<double> generateKernel(short side);
    QVector<double> normalizeMatrix(QVector<double> k, short size);

    QVector<double> manual5x5Kernel();
    QVector<double> manual3x3Kernel();

    /* Convolution function */
    QVector<QRgb> convolveImage(QImage* img, QVector<double> kernel,
				short kernel_size);
    QRgb convolveImageAtPixel(QVector<QRgb> img, short img_width,
			      QVector<double> kernel, short kernel_size,
			      int idx);
    QVector<QRgb> vectorOfImagePixels(QImage* img);
    void qimageFromPixelVector(QVector<QRgb> imgvec, short width,
				  short height, QImage::Format format);

    /* Math to determine kernel values */
    double laplacianOfGaussian(short size, short i);

    /* Methods to gather information from matrices */
    double sumOfMatrix(QVector<double> k, short size);
    double posOfMatrix(QVector<double> k, short size);
    double negOfMatrix(QVector<double> k, short size);
    double maxOfMatrix(QVector<double> k, short size);

    /* private variables */
    Ui::MainWindow *ui;

    /* Private constants */
    static double sigma;
    static const double PI;
    static const short k_size;	// kernel size (one side)

    /* The image to work with */
    QImage *image;
    QString imagePath;

    /* The display area for the image of interest */
    QLabel *displayPane;

    /**
     * This is the kernel used to apply image filters.
     * All double-arrays or matrices are going to be stored in a
     * single list format in a QVector<int>. This means items are
     * to be addressed as follows:
     * (x, y)     becomes  ==>   kernel[width*y + x]
     *            and, conversely
     * kernel[i]  becomes  ==>   (i%width, i/width)
     *
     * These can be centered around the matrix's "origin" with the
     * following scheme:
     * kernel[i]  becomes  ==>   (abs(i%size - size/2), abs(i/size - size/2))
     */
    QVector<double> kernel;

    /* Frame layout variables */
    QHBoxLayout *mainLayout;
    QVBoxLayout *imageControlBox;
    QVBoxLayout *displayBox;

    /* Button layout variables */
    QPushButton* loadBitMapButton;
    QPushButton* displayBitMapButton;
};

#endif // MAINWINDOW_H
