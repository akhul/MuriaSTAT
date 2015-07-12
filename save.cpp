#include "save.h"
#include "mainwindow.h"

Save::Save(RInside &R):
    R(R)
{

}

Save::~Save()
{

}

void Save::setActiveDataSet(ActiveDataset *ads)
{
    activeDatasetClass = ads;
}

void Save::setRCoreClass(RCore *rc)
{
    rCore = rc;
}

void Save::setMainWindow(MainWindow *mw)
{
    mainWindow = mw;
}

void Save::saveFile()
{
    qDebug("Save File");
    dataEditor = activeDatasetClass->getDataEditor();
    Rcpp::DataFrame DF = dataEditor->getDataFrame();
    QString dataName, extension, path;

    bool saveSame = false;
    QStringList datasetNameList = activeDatasetClass->getDatasetNameList();
    QStringList extensionList = activeDatasetClass->getExtList();
    QStringList pathList = activeDatasetClass->getPathList();
    for(int i=0; i<datasetNameList.size(); i++){
       QString title = mainWindow->windowTitle().split(".").at(0);
       if(title == datasetNameList[i]){
           saveSame = true;
           dataName = datasetNameList[i];
           extension = extensionList[i];
           path = pathList[i];
           break;

       }
    }
    if(extension == "" || path == "" || !saveSame){ //Belum ada di active dataset --> Save As
        saveAs();
        return;
    }

    int nRow, nCol;
    nCol = DF.size();
    Rcpp::CharacterVector vector = DF[0];
    nRow = vector.size();

    qDebug("Save Sama");
    //Save data ke R
    R[dataName.toStdString()] = DF;

    //update info activeDataset
    qDebug("Update Info");
    Rcpp::StringVector colNames;
    for(int i=0; i<nCol; i++){
        colNames.push_back(dataEditor->horizontalHeaderItem(i)->text().toStdString());
    }
    int rowIndex = activeDatasetClass->getDatasetRowIndex(dataName);
    if(rowIndex != -1){
        Rcpp::StringVector colNames = R.parseEval("colnames("+dataName.toStdString()+")");
        activeDatasetClass->updateRow(rowIndex,colNames,dataName,dataName,extension,path,nCol,nRow);
    }

    qDebug("save Data ke File");
    //save Data ke File
    std::string run;
    QString pathWOExt = path.split(".").at(0);
    qDebug(pathWOExt.toLocal8Bit());
    if(extension == "txt"){
        run = "write.table("+dataName.toStdString()+", file='"+path.toStdString()+"',sep='\\t', row.names=F)";
    }else if(extension == "csv"){
        run = "write.csv("+dataName.toStdString()+", '"+path.toStdString()+"', row.names=F)";
    }else if(extension == "sav"){
        //Masih error ?
        //run = "write.foreign("+dataName.toStdString()+", '"+pathWOExt.toStdString()+".dat', '"+pathWOExt.toStdString()+".sps', package = 'SPSS')";
    }else if(extension == "xpt"){
        //Belum dicoba
        //run = "write.foreign("+dataName.toStdString()+", '"+path.toStdString()+"', package = 'SAS')";
    }else if(extension == "dta"){
        run = "write.dta("+dataName.toStdString()+", '"+path.toStdString()+"')";
    }
    if(!run.empty()){
        R.parseEvalQ(run);
    }

    mainWindow->setWindowModified(false);
    dataEditor->clearIdxChanged();
    //activeDatasetClass->reloadData(dataName);
}

void Save::saveAs()
{
    QString path = QFileDialog::getSaveFileName(mainWindow,
                                              "Save File",
                                              ".",
                                              "Csv files (*.csv)"
                                              ";;Text files (*.txt)"
                                              ";;SPSS files (*.sav)"
                                              ";;SAS files (*.xpt)"
                                              ";;STATA files (*.dta)"
                                              ";;All files (*.*)");

    if (!path.isEmpty()){
        QString dataName = mainWindow->windowTitle().split(".").at(0);
        QString newDataName = QFileInfo(path).baseName();
        QString extension = path.split(".").at(1);
        //QString pathWOExt = path.split(".").at(0);
        qDebug("Save As\nPath:");
        qDebug(path.toLocal8Bit());

        //Save data ke R
        //Set TableItem (DataEditor) ke DataFrame jika user mengedit di Data Editor
        //jika tidak, set di R DF yang baru dari DF yg lama
        std::string run;
        Rcpp::StringVector colNames;
        dataEditor = activeDatasetClass->getDataEditor();
        Rcpp::DataFrame DF = dataEditor->getDataFrame();
        int nRow, nCol;
        nCol = DF.size();
        Rcpp::CharacterVector vector = DF[0];
        nRow = vector.size();
        //Save data ke R
        R[newDataName.toStdString()] = DF;
        run = newDataName.toStdString()+"<- as.data.frame("+newDataName.toStdString()+")";
        R.parseEvalQ(run);

        //save Data ke File
        if(extension == "txt"){
            run = "write.table("+newDataName.toStdString()+", file='"+path.toStdString()+"',sep='\\t', row.names=F)";
        }else if(extension == "csv"){
            run = "write.csv("+newDataName.toStdString()+", '"+path.toStdString()+"', row.names=F)";
        }else if(extension == "sav"){
            //Masih error ?
            //run = "write.foreign("+newDataName.toStdString()+", '"+pathWOExt.toStdString()+".dat', '"+pathWOExt.toStdString()+".sps', package = 'SPSS')";
        }else if(extension == "xpt"){
            //Belum dicoba
            //run = "write.foreign("+newDataName.toStdString()+", '"+path.toStdString()+"', package = 'SAS')";
        }else if(extension == "dta"){
            run = "write.dta("+newDataName.toStdString()+", '"+path.toStdString()+"')";
        }
        if(!run.empty()){
            //Save data from R to File
            R.parseEvalQ(run);
            qDebug("sebelum row col");
            int row, col;
            if(dataEditor->getIndexChanged().size()>0){
                row = nRow;
                col = nCol;
            }else{
                row = R.parseEval("dim("+newDataName.toStdString()+")[1]");
                col = R.parseEval("dim("+newDataName.toStdString()+")[2]");
            }
            qDebug("lewat row col");
            //Update windowTitle
            mainWindow->setCurrentFile(newDataName+"."+extension);
            //update activeDataset (Nama dataset, info, update datasetNameList, extensionList)
            bool ada = false;

            qDebug("Update Active DataSet");
            int rowIndex = activeDatasetClass->getDatasetRowIndex(dataName);
            if(rowIndex != -1){
                ada = true;
                Rcpp::StringVector colNames = R.parseEval("colnames("+newDataName.toStdString()+")");
                activeDatasetClass->updateRow(rowIndex,colNames,dataName,newDataName,extension,path,col,row);

                if(dataName != newDataName){
                    run = dataName.toStdString()+"<- NULL";
                    R.parseEvalQ(run);
                }
            }

            if(!ada){
                qDebug("Add New Active DataSet");
                activeDatasetClass->addRowData(colNames,newDataName,extension,path,col,row);
            }
            mainWindow->setWindowModified(false);
            dataEditor->clearIdxChanged();
            //activeDatasetClass->reloadData(newDataName);
        }
    }
}

void Save::saveToR()
{
    qDebug("Save to R");
    dataEditor = activeDatasetClass->getDataEditor();
    Rcpp::DataFrame DF = dataEditor->getDataFrame();
    QString dataName = mainWindow->windowTitle().split(".").at(0);

    qDebug("set DF to R");
    R[dataName.toStdString()] = DF;

    for(int i=0; i< activeDatasetClass->getDatasetNameList().size(); i++){
        if(activeDatasetClass->getDatasetNameList().at(i) == dataName){
            activeDatasetClass->updateDSModifiedList(i);
            break;
        }
    }
}
