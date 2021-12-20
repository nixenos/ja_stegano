#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <src/logic.h>
#include <iostream>
#include <string>
#include <vector>
#include <QLabel>
#include <src/dataStructures.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void on_saveFileButton_clicked();
    void on_loadDataFileButton_clicked();
    void on_loadPhotoButton_clicked();

    void on_threadsSlider_sliderMoved(int position);

    void on_chooseNewFileButton_clicked();

    void on_exitButton_clicked();

    void on_readFromBMPRadioButton_clicked();

    void on_writeDataToBMPRadioButton_clicked();

private:
    Ui::MainWindow *ui;
    std::string sourceImageFilePath;
    std::string sourceDataFilePath;
    std::string resultImageFilePath="new_image.bmp";
    std::string resultDataFilePath;
    uint8_t *pixelArray = 0;
    uint8_t *datafileArray = 0;
    BMPHEADER sourceImageBMPHeader;
    DIBHEADER sourceImageDIBHeader;
    BITFIELDS sourceImageBitFields;
    SGHEADER newImageSGHeader;
    FILE *sourceImageFile = 0;
    FILE *sourceDataFile = 0;
    FILE *destinationImageFile = 0;
    FILE *destinationDataFile = 0;
    int threadsCount = 1;
    int sourceFileSize;
    int datafileArraySize;
    int algoSelection = 0;
};
#endif // MAINWINDOW_H
