#include "open.h"
#include "dataeditor.h"
#include "rcore.h"
#include "variableeditor.h"
#include "openpreview.h"


Open::Open(QWidget *parent) :
    QWidget(parent)
{

}

Open::~Open()
{

}

void Open::setExtension(QString ext){
    extension = ext;
}
void Open::setFileName(QString fileNames){
    fileName=fileNames;
}
void Open::setPath(QString filePath){
    path = filePath;
}


QString Open::getExtension(){
    return extension;
}
QString Open::getFileName(){
    return fileName;
}

QString Open::getPath(){
    return path;
}

void Open::openFileDialog(){
    //update 27 juni .dpt --> .dta (STATA)
    QString file = QFileDialog::getOpenFileName(this,
                                              "Select source file",
                                              ".",
                                              "Text files (*.csv)"
                                              ";;csv files (*.txt)"
                                              ";;csv files (*.sav)"
                                              ";;csv files (*.xpt)"
                                              ";;csv files (*.dta)"
                                              ";;All files (*.*)");

   file.replace("/","//");
   setFileName(QFileInfo(file).baseName());
   setExtension(QFileInfo(file).suffix());
   setPath(file);

}

void Open::openNewData(RCore* rExe, ActiveDataset* activeDatasetClass){
    //update 27 juni .dpt --> .dta (STATA)
    openNew:
    QString file = QFileDialog::getOpenFileName(this,
                                              "Select source file",
                                              ".",
                                              "Csv files (*.csv)"
                                              ";;Text files (*.txt)"
                                              ";;SPSS files (*.sav)"
                                              ";;SAS files (*.xpt)"
                                              ";;STATA files (*.dta)"
                                              ";;All files (*.*)");

    if (!file.isEmpty()){
        file.replace("/","//");
        //---Update 30 Juni---
        QStringList datasetList = activeDatasetClass->getDatasetNameList();
        for(int i=0; i<datasetList.size(); i++){
            QString dataName = QFileInfo(file).baseName().replace(" ","_");
            dataName.replace("-","_");
            if(datasetList[i] == dataName){
                qDebug("sama");
                QMessageBox::StandardButton saveMessage;
                saveMessage = QMessageBox::question(this, "Dataset name duplicate", "Replace existing dataset?",
                                                QMessageBox::Yes|QMessageBox::No);
                if (saveMessage == QMessageBox::No) {
                    qDebug("No");
                    goto openNew;
                }else if(saveMessage == QMessageBox::Yes){
                    qDebug("Yes");
                    for(int i=0; i< activeDatasetClass->rowCount(); i++){
                        if(!activeDatasetClass->verticalHeaderItem(i)->text().isEmpty()){
                            if(activeDatasetClass->item(i,2)->text() == dataName){
                                activeDatasetClass->deleteDataset(i);
                            }
                        }
                    }
                }
            }
        }
        QString dataName = QFileInfo(file).baseName().replace(" ","_");
        dataName.replace("-","_");
        setFileName(dataName);
        //----
        setExtension(QFileInfo(file).suffix());
        setPath(file);

        QFile file(getPath());
        if(!file.open(QIODevice::ReadOnly)){
            QMessageBox::information(0, "info", file.errorString());
        }
        QTextStream in(&file);
        /* QString temp;
        QStringList lineList;
        QList <int> lengthLine;
        while(!in.atEnd()){
           temp=in.readLine();
           lineList = temp.split(";");
           lengthLine.append(lineList.size());
        }*/

        OpenPreview *dataPreview = new OpenPreview;
        //dataPreview->setInputfile(in.readAll());
        dataPreview->show();

        if(dataPreview->exec()==QDialog::Accepted){
        rExe->OpenData(path.toStdString(),fileName.toStdString(),extension.toStdString());
        activeDatasetClass->addRowData(rExe->getColNames(),fileName,extension,path,rExe->getCol(),rExe->getRow());}
    }else{
        //Update 27 Juni
        return;
    }


}
