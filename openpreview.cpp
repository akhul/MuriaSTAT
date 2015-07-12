#include "openpreview.h"
#include "ui_openpreview.h"

OpenPreview::OpenPreview(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OpenPreview)
{
    ui->setupUi(this);
}

OpenPreview::~OpenPreview()
{
    delete ui;
}

void OpenPreview::setInputfile(QString data){
    ui->inputFileTextEdit->setText(data);
}
void OpenPreview::setMissing(QString miss){
    missing=miss;
}
void OpenPreview::setSeparator(QChar sep){
    separator=sep;

        ui->separatorComboBox->setCurrentIndex(2);


}
void OpenPreview::setHeading(QString heading){
    header=heading;
}

void OpenPreview::setComma(QString com){
    comma=com;
}
void OpenPreview::setDecimal(QString dec){
    decimal=dec;
}
void OpenPreview::setQuote(QString quot){
    quote=quot;
}

QString OpenPreview::getHeading(){
    return header;
}
QString OpenPreview::getComma(){
    return comma;
}
QString OpenPreview::getDecimal(){
    return decimal;
}
QString OpenPreview::getQuote(){
    return quote;
}
QChar OpenPreview::getSeparator(){
    return separator;
}
QString OpenPreview::getDatasetName(){
    return datasetName;
}
