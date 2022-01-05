#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include "src/dataStructures.h"
#include "src/logic.h"
#include <chrono>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_loadDataFileButton_clicked()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("All files (*.*)"));
    dialog.setViewMode(QFileDialog::Detail);
    QStringList filenamesTemp;
    if (dialog.exec())
        filenamesTemp = dialog.selectedFiles();
    if (filenamesTemp.size() > 1) {
        return;
    } else {
        if (filenamesTemp.size() > 0) {
            this->sourceDataFilePath = filenamesTemp[0].toStdString();
            ui->dataFileNameLabel->setText("Source data file location: " + filenamesTemp[0]);
        } else {
            return;
        }
    }
    this->sourceDataFile = fopen(sourceDataFilePath.c_str(), "rb");
    if (this->sourceDataFile) {
        datafileArray = read_file_to_memory(this->sourceDataFile, &this->sourceFileSize, &this->datafileArraySize);
    }
}

void MainWindow::on_loadPhotoButton_clicked()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("Images (*.bmp *.BMP *.bmp)"));
    dialog.setViewMode(QFileDialog::Detail);
    QStringList filenamesTemp;
    if (dialog.exec())
        filenamesTemp = dialog.selectedFiles();
    if (filenamesTemp.size() > 1) {
        return;
    }
    if (filenamesTemp.size() > 0){
        this->sourceImageFilePath = filenamesTemp[0].toStdString();
        ui->photoLocationLabel->setText("Source photo location: " + filenamesTemp[0]);
    } else {
        return;
    }
    this->sourceImageFile = fopen(sourceImageFilePath.c_str(), "rb");
    if(read_headers(sourceImageFile, &sourceImageBMPHeader, &sourceImageDIBHeader, &sourceImageBitFields)==0){
        printf("Błąd odczytu nagłówków z pliku!\n");
        fclose(sourceImageFile);
        return;
    }

}


void MainWindow::on_saveFileButton_clicked() {
    if (ui->writeDataToBMPRadioButton->isChecked()) {
        if (ui->cAPICheckBox->isChecked()) {
            if (sourceImageFile) {
                uint32_t pixelCount = this->sourceImageDIBHeader.WIDTH * this->sourceImageDIBHeader.HEIGHT;
                switch (this->sourceImageDIBHeader.BITSPERPIXEL) {
                    case 16:
                        this->pixelArray = pixelArray_read_16bit(sourceImageFile, pixelCount);
                        break;
                    case 24:
                        this->pixelArray = pixelArray_read_24bit(sourceImageFile, pixelCount);
                        break;
                    case 32:
                        this->pixelArray = pixelArray_read_32bit(sourceImageFile, pixelCount);
                        break;
                    default:
                        this->pixelArray = 0;
                        break;
                }
                if (this->pixelArray && this->datafileArray && destinationImageFile) {
                    auto t1 = std::chrono::high_resolution_clock::now();
                    int res = write_data_to_image(pixelArray, datafileArray, sourceImageBMPHeader, sourceImageDIBHeader,
                                        destinationImageFile, sourceFileSize, 2,
                                        sourceImageDIBHeader.BITSPERPIXEL,
                                        sourceImageDIBHeader.HEIGHT * sourceImageDIBHeader.WIDTH,
                                        datafileArraySize,
                                        sourceImageBitFields, threadsCount, 1);
                    auto t2 = std::chrono::high_resolution_clock::now();
                    auto ms_count = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
                    std::string tempString;
                    tempString = "Time with C++: " + std::to_string(ms_count.count()) + " ms";
                    ui->label->setText(tempString.c_str());
                    QMessageBox msgBox;
                    if (res) {
                        msgBox.setText("Image has been sucessfully saved!");
                    }
                    else {
                        msgBox.setText("Error calculating new image data!");
                    }
                    msgBox.exec();
                }
            }
        }
        if (ui->ASMAPICheckBox->isChecked()) {
            if (sourceImageFile) {
                uint32_t pixelCount = this->sourceImageDIBHeader.WIDTH * this->sourceImageDIBHeader.HEIGHT;
                switch (this->sourceImageDIBHeader.BITSPERPIXEL) {
                    case 16:
                        this->pixelArray = pixelArray_read_16bit(sourceImageFile, pixelCount);
                        break;
                    case 24:
                        this->pixelArray = pixelArray_read_24bit(sourceImageFile, pixelCount);
                        break;
                    case 32:
                        this->pixelArray = pixelArray_read_32bit(sourceImageFile, pixelCount);
                        break;
                    default:
                        this->pixelArray = 0;
                        break;
                }
                if (this->pixelArray && this->datafileArray && destinationImageFile) {
                    auto t1 = std::chrono::high_resolution_clock::now();
                    int res = write_data_to_image(pixelArray, datafileArray, sourceImageBMPHeader, sourceImageDIBHeader,
                                        destinationImageFile, sourceFileSize, 2,
                                        sourceImageDIBHeader.BITSPERPIXEL,
                                        sourceImageDIBHeader.HEIGHT * sourceImageDIBHeader.WIDTH,
                                        datafileArraySize,
                                        sourceImageBitFields, threadsCount, 0);
                    auto t2 = std::chrono::high_resolution_clock::now();
                    auto ms_count = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
                    std::string tempString;
                    tempString = "Time with ASM: " + std::to_string(ms_count.count()) + " ms";
                    ui->label_2->setText(tempString.c_str());
                    QMessageBox msgBox;
                    if (res) {
                        msgBox.setText("Image has been sucessfully saved!");
                    }
                    else {
                        msgBox.setText("Error calculating new image data!");
                    }
                    msgBox.exec();
                }
            }
        }
    }
        if (ui->readFromBMPRadioButton->isChecked()) {
            if (ui->cAPICheckBox->isChecked()) {
            //if (true) {
                if (sourceImageFile) {
                    uint32_t pixelCount = this->sourceImageDIBHeader.WIDTH * this->sourceImageDIBHeader.HEIGHT;
                    switch (this->sourceImageDIBHeader.BITSPERPIXEL) {
                        case 16:
                            this->pixelArray = pixelArray_read_16bit(sourceImageFile, pixelCount);
                            break;
                        case 24:
                            this->pixelArray = pixelArray_read_24bit(sourceImageFile, pixelCount);
                            break;
                        case 32:
                            this->pixelArray = pixelArray_read_32bit(sourceImageFile, pixelCount);
                            break;
                        default:
                            this->pixelArray = 0;
                            break;
                    }
                    if (this->pixelArray) {
                        if (check_for_stegano(sourceImageBMPHeader)) {
                            int result = write_data_from_image(sourceImageBMPHeader, pixelArray, destinationImageFile,
                                                               sourceImageDIBHeader, 2, 1, threadsCount);
                            if (result) {
                                std::cout << "Success\n";
                                QMessageBox msgBox;
                                msgBox.setText("Data file has been sucessfully saved!");
                                msgBox.exec();
                            } else {
                                QMessageBox msgBox;
                                msgBox.setText("Error calculating new data file!");
                                msgBox.exec();
                                std::cout << "Fails\n";
                            }
                        } else {
                            std::cout << "No stegano header\n";
                        }
                    } else {
                        std::cout << "No pixel array\n";
                    }
                } else {
                    std::cout << "No source image pointer\n";
                }
            }

            if (ui->ASMAPICheckBox->isChecked()){
                if (sourceImageFile) {
                    uint32_t pixelCount = this->sourceImageDIBHeader.WIDTH * this->sourceImageDIBHeader.HEIGHT;
                    switch (this->sourceImageDIBHeader.BITSPERPIXEL) {
                        case 16:
                            this->pixelArray = pixelArray_read_16bit(sourceImageFile, pixelCount);
                            break;
                        case 24:
                            this->pixelArray = pixelArray_read_24bit(sourceImageFile, pixelCount);
                            break;
                        case 32:
                            this->pixelArray = pixelArray_read_32bit(sourceImageFile, pixelCount);
                            break;
                        default:
                            this->pixelArray = 0;
                            break;
                    }
                    if (this->pixelArray) {
                        if (check_for_stegano(sourceImageBMPHeader)) {
                            int result = write_data_from_image(sourceImageBMPHeader, pixelArray, destinationImageFile,
                                                               sourceImageDIBHeader, 2, 0, threadsCount);
                            if (result) {
                                std::cout << "Success\n";
                                QMessageBox msgBox;
                                msgBox.setText("Data file has been sucessfully saved!");
                                msgBox.exec();
                            } else {
                                QMessageBox msgBox;
                                msgBox.setText("Error calculating new data file!");
                                msgBox.exec();
                                std::cout << "Fails\n";
                            }
                        } else {
                            std::cout << "No stegano header\n";
                        }
                    } else {
                        std::cout << "No pixel array\n";
                    }
                } else {
                    std::cout << "No source image pointer\n";
                }
            }
        }
    sourceImageFilePath = "";
    sourceDataFilePath = "";
    ui->photoLocationLabel->setText("Source photo location: ");
    ui->dataFileNameLabel->setText("Source data file location: ");
    if (pixelArray) {
        free(pixelArray);
        pixelArray = 0;
    }
    if (datafileArray) {
        free(datafileArray);
        datafileArray = 0;
    }
    if (sourceImageFile) {
        fclose(sourceImageFile);
        sourceImageFile = 0;
    }
    if (sourceDataFile) {
        fclose(sourceDataFile);
        sourceDataFile = 0;
    }
    if (destinationImageFile) {
        fclose(destinationImageFile);
        destinationImageFile = 0;
        ui->newFileNameLabel->setText("New file name: ");
    }
}

void MainWindow::on_threadsSlider_sliderMoved(int position){
    this->threadsCount = position;
    std::string temp = "Threads: " + std::to_string(this->threadsCount);
    ui->threadsLabel->setText(temp.c_str());
}


void MainWindow::on_chooseNewFileButton_clicked() {
    bool ok;
    QString text = QInputDialog::getText(this, tr("Result image name"),
                                         tr("New file name: "), QLineEdit::Normal,
                                         "new_image.bmp", &ok);
    if (ok && !text.isEmpty()) {
        resultImageFilePath = text.toStdString();
        if (!resultImageFilePath.empty()) {
            ui->newFileNameLabel->setText("Result file location: " + text);
        } else {
            return;
        }
    }
    remove(resultImageFilePath.c_str());
    this->destinationImageFile = fopen(resultImageFilePath.c_str(), "wb+");
}

void MainWindow::on_exitButton_clicked(){
    exit(EXIT_SUCCESS);
}


void MainWindow::on_readFromBMPRadioButton_clicked(){
    sourceImageFilePath = "";
    sourceDataFilePath = "";
    ui->photoLocationLabel->setText("Source photo location: ");
    ui->dataFileNameLabel->setText("Source data file location: ");
    if (pixelArray) {
        free(pixelArray);
        pixelArray = 0;
    }
    if (datafileArray) {
        free(datafileArray);
        datafileArray = 0;
    }
    if (sourceImageFile) {
        fclose(sourceImageFile);
        sourceImageFile = 0;
    }
    if (sourceDataFile) {
        fclose(sourceDataFile);
        sourceDataFile = 0;
    }
    if (destinationImageFile) {
        fclose(destinationImageFile);
        destinationImageFile = 0;
        ui->newFileNameLabel->setText("New file name: ");
    }
    ui->loadDataFileButton->setEnabled(false);
    //ui->ASMAPICheckBox->setEnabled(false);
    //ui->cAPICheckBox->setEnabled(false);
}


void MainWindow::on_writeDataToBMPRadioButton_clicked(){
    sourceImageFilePath = "";
    sourceDataFilePath = "";
    ui->photoLocationLabel->setText("Source photo location: ");
    ui->dataFileNameLabel->setText("Source data file location: ");
    if (pixelArray) {
        free(pixelArray);
        pixelArray = 0;
    }
    if (datafileArray) {
        free(datafileArray);
        datafileArray = 0;
    }
    if (sourceImageFile) {
        fclose(sourceImageFile);
        sourceImageFile = 0;
    }
    if (sourceDataFile) {
        fclose(sourceDataFile);
        sourceDataFile = 0;
    }
    if (destinationImageFile) {
        fclose(destinationImageFile);
        destinationImageFile = 0;
        ui->newFileNameLabel->setText("New file name: ");
    }
    ui->loadDataFileButton->setEnabled(true);
    //ui->ASMAPICheckBox->setEnabled(true);
    //ui->cAPICheckBox->setEnabled(true);
}

