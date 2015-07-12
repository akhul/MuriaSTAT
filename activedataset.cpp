#include "activedataset.h"
#include "RInside.h"
#include "buttoncustom.h"
#include "mainwindow.h"

ActiveDataset::ActiveDataset(QWidget *parent, DataEditor *dataEditor2, VariableEditor *variableEditor2) :
    QTableWidget(parent),dataEditor(dataEditor2),variableEditor(variableEditor2)
{
    setColumnCount(4);
    setRowCount(0);
    setHorizontalHeaderItem(0, new QTableWidgetItem(""));
    setHorizontalHeaderItem(1, new QTableWidgetItem(""));
    setHorizontalHeaderItem(2, new QTableWidgetItem("Dataset"));
    setHorizontalHeaderItem(3, new QTableWidgetItem("Info"));
    setColumnWidth(0,25);
    setColumnWidth(1,25);
    setColumnWidth(2,150);
    setColumnWidth(3,200);
    verticalHeader()->setDefaultSectionSize(30);
    numberTable=0;

    //Update 27 Juni
    mainWindow = (MainWindow*)parent;

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)),
        this, SLOT(showContextMenu(const QPoint&))); //Untuk klik kanan, delete

}

ActiveDataset::~ActiveDataset()
{

}
void ActiveDataset::setMainEditor(QWidget* w){
    mainEditor =w;
}
void ActiveDataset::setMainOutput(QWidget* w){
    mainOutput =w;
}
void ActiveDataset::setMainText(QPlainTextEdit* w){
    mainPlainText =w;
}
void ActiveDataset::setRcore(RCore *r){
    rCore=r;
}
void ActiveDataset::setDataEditor(DataEditor *de)
{
    dataEditor = de;
}
void ActiveDataset::setVarEditor(VariableEditor *ve)
{
    variableEditor = ve;
}

void ActiveDataset::addRowData(Rcpp::StringVector colNames, QString name, QString ext, QString path, int col, int row){
    qDebug("Add row Data");
    QString stringTemp;

    int buttonIndex=rowCount();
    insertRow(buttonIndex);
    numberTable = numberTable+1;

    setVerticalHeaderItem(buttonIndex,new QTableWidgetItem(QString::number(numberTable)));
    ButtonCustom *showButton = new ButtonCustom;
    QIcon import;
    import.addFile(QString::fromUtf8(":/image/openData.png"), QSize(), QIcon::Normal, QIcon::Off);
    QPushButton*importButton = new QPushButton;
    importButton->setIcon(import);
    importButton->setStyleSheet( "background-color: rgba( 255, 255, 255, 0% );" );
    showButton->setCollapseIcon();

    showButton->setStyleSheet( "background-color: rgba( 255, 255, 255, 0% );" );

    setCellWidget(buttonIndex,0,showButton);
    setCellWidget(buttonIndex,1,importButton);

    //Update 29 Juni
    QTableWidgetItem *it = new QTableWidgetItem;
    it = new QTableWidgetItem;
    it->setText(name);
    it->setFlags(it->flags() & ~Qt::ItemIsEditable);
    setItem(buttonIndex,2,it);
    QString info = QString::number(col) +" variable and "+QString::number(row)+" Observation";
    it = new QTableWidgetItem;
    it->setText(info);
    it->setFlags(it->flags() & ~Qt::ItemIsEditable);
    setItem(buttonIndex,3,it);

    int index=rowCount();
    start=index;

    //==update==//
    stringTemp.append(name);

    for (int i = 0; i <colNames.size(); ++i) {
        insertRow(index);
        setVerticalHeaderItem(index,new QTableWidgetItem(QString("")));
        //---Update 29 Juni
        it = new QTableWidgetItem;
        it->setText("\t"+QString(colNames[i]));
        it->setFlags(it->flags() & ~Qt::ItemIsEditable);
        setItem(index,0,it);
        it = new QTableWidgetItem;
        it->setText("");
        it->setFlags(it->flags() & ~Qt::ItemIsEditable);
        setItem(index,3,it);
        //---
        setSpan(index,0,1,3);
        item(index,0)->setForeground(QColor::fromRgb(255,255,255));//
        setRowHidden(index,true);//
        ++index;
        stringTemp.append(","+QString(colNames[i]));
     }

    end= index;

    //==update==//
    setDatasetName(name);
    setVarName(stringTemp);

    registerSlotHide(showButton, buttonIndex);
    registerSlot(importButton,buttonIndex);

    //Update 26 Juni
    extensionList.append(ext);
    pathList.append(path);
    dataSetModifiedList.append(false);
    qDebug(path.toLocal8Bit());
}

void ActiveDataset::activeLoad(int index){
    qDebug("Active Load");
    //Cek ada perubahan apa gak, kalo ada tapi belum dibuat dataset, tawari save. Jika udah ada dataset, langsung save ke R
    mainWindow->saveToR();

    QString dataName = item(index,2)->text();

    //Update 26 Juni
    QString title = mainWindow->windowTitle().split(".").at(0);
    if(mainEditor->isHidden() || title != dataName || !dataEditor->item(0,0)){
        qDebug("----Beda");

        TabEditor *newTab = new TabEditor;
        //---Update 26 Juni---
        delete dataEditor;
        dataEditor = new DataEditor;
        mainWindow->setDataEditorConnect(dataEditor);

        delete variableEditor;
        variableEditor = new VariableEditor;
        variableEditor->setActiveDataset(this);
        variableEditor->setDataEditor(dataEditor);
        variableEditor->setDatasetName(dataName);

        for(int i=0; i<datasetNameList.size(); i++){
            if(datasetNameList[i] == dataName){
                mainWindow->setCurrentFile(dataName+"."+extensionList[i]);
            }
        }
        //--- Update 25 Juni
        newTab->displayEditor(rCore,dataName.toStdString(), dataEditor, variableEditor);

        mainOutput->hide();
        mainPlainText->hide();
        mainEditor->show();

        QHBoxLayout *layout = new QHBoxLayout;
        layout->addWidget(newTab);
        if(mainEditor->layout()){
            qDebug("ada layout");
            delete mainEditor->layout();
        }
        mainEditor->setLayout(layout);
        mainWindow->disableActionSave();
    }

    //QMessageBox::information(this,"","Tes");
}

void ActiveDataset::activeHideExpand(int idx){
    QColor col = QColor::fromRgb(255,255,255);
    QColor col2 = QColor::fromRgb(255,0,255);
    int index=idx+1;

    QPushButton* nameButtonStatus=new QPushButton(cellWidget(idx,0));
    if(nameButtonStatus->icon().themeName()==QString("collapse")){
        while(verticalHeaderItem(index)->text()==""){
            item(index,0)->setForeground(col);
            setRowHidden(index,true);

            if(index<rowCount()-1){
                index++;
                }
            else{
                    break;
            }

        }
    }
    index=idx+1;
    if(nameButtonStatus->icon().themeName()==QString("expand")){
        while(verticalHeaderItem(index)->text()==""){
            if(item(index,0)->foreground()==col){
                item(index,0)->setForeground(col2);
                showRow(index);
            }
            if(index<rowCount()-1){
                index++;
                }
            else{
                    break;
            }
        }
    }
}

void ActiveDataset::changeButton(QPushButton* button,int index){
    statusButton = button->icon().themeName();
    if(statusButton==QString("collapse")){

      }
}

void ActiveDataset::registerSlot(QPushButton *button,int index){
    signalMapper = new QSignalMapper(button);
    connect(button,SIGNAL(clicked()),signalMapper,SLOT(map()));
    signalMapper->setMapping(button,index);
    connect(signalMapper,SIGNAL(mapped(int)),this,SLOT(activeLoad(int)));

}

void ActiveDataset::registerSlotHide(QPushButton *button,int index){
    signalMapper = new QSignalMapper(button);
    connect(button,SIGNAL(clicked()),signalMapper,SLOT(map()));
    signalMapper->setMapping(button,index);
    connect(signalMapper,SIGNAL(mapped(int)),this,SLOT(activeHideExpand(int)));

}
void ActiveDataset::registerSlotChange(ButtonCustom *button,int index){
    signalMapper2 = new QSignalMapper(button);
    connect(button,SIGNAL(clicked()),signalMapper,SLOT(map()));
    signalMapper2->setMapping(button,index);
    connect(signalMapper2,SIGNAL(mapped(int)),button,SLOT(setExpandIcon()));

}

//==update==//
void ActiveDataset::setDatasetName(QString datasetName){
    datasetNameList<<datasetName;
}
QStringList ActiveDataset::getDatasetNameList(){
    return datasetNameList;
}

void ActiveDataset::setVarName(QString var){
    varList<<var;
}
QStringList ActiveDataset::getVarList(){
    return varList;
}

//Update 26 Juni
void ActiveDataset::updateAllList(QString oldName, QString newName,QString newExt, QString newPath)
{
    for(int i=0; i<datasetNameList.size(); i++){
        if(datasetNameList[i] == oldName){
            datasetNameList.replace(i,newName);
            extensionList.replace(i,newExt);
            pathList.replace(i,newPath);
        }
    }
}

DataEditor* ActiveDataset::getDataEditor()
{
    return dataEditor;
}
VariableEditor* ActiveDataset::getVarEditor()
{
    return variableEditor;
}

QStringList ActiveDataset::getExtList(){
    return extensionList;
}

QStringList ActiveDataset::getPathList()
{
    return pathList;
}

void ActiveDataset::reloadData(QString dataName){
    qDebug("*** Active load (name) ***");

    for(int i=0; i<datasetNameList.size(); i++){
        if(datasetNameList[i] == dataName){
            mainWindow->setCurrentFile(dataName+"."+extensionList[i]);
        }
    }

    TabEditor *newTab = new TabEditor;
    //---Update 26 Juni---
    delete dataEditor;
    dataEditor = new DataEditor;
    mainWindow->setDataEditorConnect(dataEditor);

    delete variableEditor;
    variableEditor = new VariableEditor;
    variableEditor->setActiveDataset(this);
    variableEditor->setDataEditor(dataEditor);
    //--- Update 25 Juni
    newTab->displayEditor(rCore,dataName.toStdString(), dataEditor, variableEditor);

    mainOutput->hide();
    mainPlainText->hide();
    mainEditor->show();

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(newTab);
    if(mainEditor->layout()){
        qDebug("ada layout");
        delete mainEditor->layout();
    }
    mainEditor->setLayout(layout);
    mainWindow->disableActionSave();
}

void ActiveDataset::showContextMenu(const QPoint& pos)
{
    if(itemAt(pos)){
        QPoint globalPos = mapToGlobal(pos);
        QMenu myMenu;
        myMenu.addAction("Close Dataset");

        QAction* selectedItem = myMenu.exec(globalPos);
        if (selectedItem)
        {
            QTableWidgetItem *it = itemAt(pos);
            int row = it->row();
            deleteDataset(row);
        }
    }
}

void ActiveDataset::deleteDataset(int row)
{
    qDebug("Delete dataset");
    mainWindow->saveToR();
    QString dataName;
    while(true){
        if(!verticalHeaderItem(row)->text().isEmpty()){
            dataName = item(row,2)->text();
            break;
        }else{
            row--;
        }
    }
    qDebug() << row;
    qDebug() << dataName.toLocal8Bit();
    //delete list
    int index;
    for(int i=0; i<datasetNameList.size(); i++){
        if(datasetNameList[i] == dataName){
            index = i;
            //Update 6 Juli
            qDebug() << dataSetModifiedList[i];
            if(dataSetModifiedList[i]){
                mainWindow->showSaveDialog(datasetNameList[i]);
            }
            datasetNameList.erase(datasetNameList.begin()+i);
            extensionList.erase(extensionList.begin()+i);
            pathList.erase(pathList.begin()+i);
            dataSetModifiedList.erase(dataSetModifiedList.begin()+i);
        }
    }
    //delete R
    std::string run = dataName.toStdString()+"<- NULL";
    rCore->runSyntax(run);

    //delete row dataset table
    do{
        removeRow(row);
    }while(rowCount()>row && verticalHeaderItem(row)->text() == "");
    numberTable--;
    //delete dataset dibawahnya yg diclose kalau ada
    QStringList tempVar;
    QStringList tempInfo;
    QList<int> tempNVar;
    while(true){
        qDebug("While true");
        if(verticalHeaderItem(row)){
            tempInfo.append(item(row,3)->text());
            int nVar = item(row,3)->text().split(" variable").at(0).toInt();
            tempNVar.append(nVar);
            for(int i=0; i<nVar; i++){
                tempVar.append(item(row+i+1,0)->text().replace("\t",""));
            }
        }else{
            break;
        }

        do{
            removeRow(row);
        }while(rowCount()>row && verticalHeaderItem(row)->text() == "");
        numberTable--;
    }
    //qDebug("after while true");
    //add dataset dibawahnya yg diclose kalau ada
    while(tempInfo.size() > 0){
        Rcpp::StringVector colNames;
        QString name = datasetNameList[index];
        datasetNameList.erase(datasetNameList.begin()+index);
        QString ext = extensionList[index];
        extensionList.erase(extensionList.begin()+index);
        QString path = pathList[index];
        pathList.erase(pathList.begin()+index);
        dataSetModifiedList.erase(dataSetModifiedList.begin()+index);

        int col = tempNVar[0];
        tempNVar.pop_front();
        int row = tempInfo[0].split("and").at(1).split(" Observation").at(0).toInt();
        tempInfo.pop_front();
        for(int i=0; i<col; i++){
            colNames.push_back(tempVar[0].toStdString());
            tempVar.pop_front();
        }
        addRowData(colNames,name,ext,path,col,row);
    }
    if(dataEditor && dataName == mainWindow->windowTitle().split(".").at(0)){
        dataEditor->clear();
        mainWindow->setCurrentFile("");
    }
}

void ActiveDataset::updateRow(int index, Rcpp::StringVector colNames, QString oldName, QString newName, QString ext, QString path, int col, int row)
{
    qDebug("updateVarNames");
    //delete row dataset table
    do{
        removeRow(index);
    }while(rowCount()>index && verticalHeaderItem(index)->text() == "");
    numberTable--;

    //delete dataset dibawahnya yg diupdate kalau ada
    QStringList tempVar;
    QStringList tempInfo;
    QList<int> tempNVar;
    while(true){
        qDebug("While true");
        if(verticalHeaderItem(index)){
            tempInfo.append(item(index,3)->text());
            int nVar = item(index,3)->text().split(" variable").at(0).toInt();
            tempNVar.append(nVar);
            for(int i=0; i<nVar; i++){
                tempVar.append(item(index+i+1,0)->text().replace("\t",""));
            }
        }else{
            break;
        }

        do{
            removeRow(index);
        }while(rowCount()>index && verticalHeaderItem(index)->text() == "");
        numberTable--;
    }
    //qDebug("after while true");
    //delete list
    int iList;
    for(int i=0; i<datasetNameList.size(); i++){
        if(datasetNameList[i] == oldName){
            iList = i;
            datasetNameList.erase(datasetNameList.begin()+i);
            extensionList.erase(extensionList.begin()+i);
            pathList.erase(pathList.begin()+i);
            dataSetModifiedList.erase(dataSetModifiedList.begin()+i);
        }
    }

    addRowData(colNames,newName,ext,path,col,row);
    updateDSModifiedList(dataSetModifiedList.size()-1);
    //add dataset dibawahnya yg diupdate kalau ada
    while(tempInfo.size() > 0){
        Rcpp::StringVector colNames1;
        QString name1 = datasetNameList[iList];
        datasetNameList.erase(datasetNameList.begin()+iList);
        QString ext1 = extensionList[iList];
        extensionList.erase(extensionList.begin()+iList);
        QString path1 = pathList[iList];
        pathList.erase(pathList.begin()+iList);
        dataSetModifiedList.erase(dataSetModifiedList.begin()+iList);
        int col1 = tempNVar[0];
        tempNVar.pop_front();
        int row1 = tempInfo[0].split("and").at(1).split(" Observation").at(0).toInt();
        tempInfo.pop_front();
        for(int i=0; i<col1; i++){
            colNames1.push_back(tempVar[0].toStdString());
            tempVar.pop_front();
        }
        addRowData(colNames1,name1,ext1,path1,col1,row1);
    }
}

void ActiveDataset::updateDSModifiedList(int index)
{
    qDebug("Update dataset modified");
    qDebug() << index;
    qDebug() << dataSetModifiedList.size();
    dataSetModifiedList.replace(index,true);
}

bool ActiveDataset::getDSModifiedAt(int index)
{
    return dataSetModifiedList.at(index);
}

int ActiveDataset::getDatasetIndex(QString datasetName)
{
    for(int i=0; i<datasetNameList.size(); i++){
        if(datasetName == datasetNameList[i]){
            return i;
        }
    }
    return -1;
}

int ActiveDataset::getDatasetRowIndex(QString datasetName)
{
    for(int i=0; i< rowCount(); i++){
        if(!verticalHeaderItem(i)->text().isEmpty()){
            if(item(i,2)->text() == datasetName){
                return i;
            }
        }
    }
    return -1;
}
