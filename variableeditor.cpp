#include "variableeditor.h"
#include "activedataset.h"
#include "mainwindow.h"

VariableEditor::VariableEditor(QWidget *parent):QTableWidget(parent)
{
    QStringList colNamesVE;
    colNamesVE.push_back("Name");
    colNamesVE.push_back("Label");
    colNamesVE.push_back("Type");
    colNamesVE.push_back("Decimal");
    colNamesVE.push_back("Missing");
    colNamesVE.push_back("Scale");
    colNamesVE.push_back("Note");

    setColumnCount(colNamesVE.size());
    setRowCount(20);
    setColumnWidth(0,200);
    setColumnWidth(1,190);

    for (int i = 0; i < colNamesVE.size(); i++) {
        QTableWidgetItem *item = new QTableWidgetItem;
        item->setText(QString(colNamesVE[i]));
        setHorizontalHeaderItem(i, item);
    }
    connect(this, SIGNAL(cellChanged(int,int)),this,SLOT(changeVarProp(int, int)));
    connect(this, SIGNAL(cellClicked(int,int)),this,SLOT(cellWidgetClicked(int,int)));


    verticalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(verticalHeader(),SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(showRowContextMenu(QPoint)));
}

void VariableEditor::keyPressEvent(QKeyEvent* event)
{
    //Saat enter ganti baris, tapi saat edit cell belum bisa tekan tombol kiri atau kanan untuk pindah kolom
    if(event->key() == 16777220 && currentColumn()!=3) //Key_Enter = 16777221
    {
        setCurrentCell(currentRow()+1,currentColumn());
    }
    else
    {
        QTableWidget::keyPressEvent(event);
    }
}

void VariableEditor::addNewVarVE(int col, QString newVar, QString newType)
{
    //Tambahkan variabel baru pada variabel editor dari Data Editor
    blockSignals(true);
    if(col == varNamesVE.size() || (col==0 && varNamesVE.isEmpty())){
        qDebug("addNewVarVE");
        varNamesVE.push_back(newVar);
        varTypesVE.push_back(newType);

    }else{
        qDebug("insert newvarVE");
        insertRow(col);

        varNamesVE.insert(col,newVar);
        varTypesVE.insert(col,newType);

        dataEditor = activeDataset->getDataEditor();
        dataEditor->insertNewVar(col);
        dataEditor->setVarNames(varNamesVE);
        dataEditor->setVarTypes(varTypesVE);

    }

    QTableWidgetItem *it;
    for(int k=0; k<=6; k++){
        it = new QTableWidgetItem;
        if(k==0){
            it->setText(newVar);
        }if(k==2){
            it->setText(newType);
        }else if(k==3){
            if(newType == "Real"){
                it->setText("2");
            }else{
                it->setText("0");
            }
        }else if(k==4){
            it->setText("Yes");
        }
        setItem(col,k,it);
    }

    //Update active dataset
    qDebug("Update Active DataSet");
    QString extension, path;
    int index = activeDataset->getDatasetIndex(datasetName);
    if(index != -1){
        path = activeDataset->getPathList().at(index);
        extension  = activeDataset->getExtList().at(index);
        int rowIndex = activeDataset->getDatasetRowIndex(datasetName);
        if(rowIndex != -1){
            int row =activeDataset->item(rowIndex,3)->text().split("and ").at(1).split(" Observation").at(0).toInt();
            int col=varNamesVE.size();
            Rcpp::StringVector colNames;
            for(int c=0; c<varNamesVE.size(); c++){
                colNames.push_back(varNamesVE[c].toStdString());
            }
            activeDataset->updateRow(rowIndex,colNames,datasetName,datasetName,extension,path,col,row);
        }
    }

    blockSignals(false);
}

void VariableEditor::addNewVar(int i, int j)
{
    //Tambahkan variabel baru pada variabel editor dari variabel editor sendiri

    blockSignals(true);
    //setRowTable(1,index);
    qDebug("add NewVar");
    if(j==0){
        qDebug("j=0");
        varNamesVE.push_back(item(i,0)->text());
    }
    else{
        QTableWidgetItem *it = new QTableWidgetItem;
        it->setText("NewVar "+QString::number(i+1));
        setItem(i,0,it);
        varNamesVE.push_back("NewVar");
        if(j==2){
            qDebug("j=2");
            if(cellWidget(i,j)){
                if(!((QComboBox*)cellWidget(i,j))->isHidden()){
                    QString newVal = ((QComboBox*)cellWidget(i,j))->currentText();
                    ((QComboBox*)cellWidget(i,j))->hide();
                    QTableWidgetItem *item = new QTableWidgetItem;
                    item->setText(newVal);
                    setItem(i,j,item);
                    varTypesVE.push_back(newVal);
                }
            }
        }
    }
    qDebug("...");
    QTableWidgetItem *it;
    for(int k=1; k<=6; k++){
        if(!item(i,k)){
            it = new QTableWidgetItem;
            if(k==2){
                it->setText("String");
                varTypesVE.push_back("String");
            }else if(k==3){
                it->setText("2");
            }else if(k==4){
                it->setText("Yes");
            }
            setItem(i,k,it);
        }
    }
    //Update ke Dataeditor
    dataEditor->setVarNames(varNamesVE);
    dataEditor->setVarTypes(varTypesVE);
    dataEditor->setHorizontalHeaderItem(i,new QTableWidgetItem(QString(varNamesVE.at(i))));
    //Update active dataset
    qDebug("Update Active DataSet");
    int index = activeDataset->getDatasetIndex(datasetName);
    if( index != -1){
        QString extension, path;
        path = activeDataset->getPathList()[j];
        extension  = activeDataset->getExtList()[j];

        int rowIndex = activeDataset->getDatasetRowIndex(datasetName);
        if(rowIndex != -1){
            int row =activeDataset->item(rowIndex,3)->text().split("and ").at(1).split(" Observation").at(0).toInt();
            int col=varNamesVE.size();
            Rcpp::StringVector colNames;
            for(int c=0; c<varNamesVE.size(); c++){
                colNames.push_back(varNamesVE[c].toStdString());
            }
            activeDataset->updateRow(rowIndex,colNames,datasetName,datasetName,extension,path,col,row);
        }
    }
    blockSignals(false);
}

void VariableEditor::loadVariable(RCore *rExe, Rcpp::StringVector colNames)
{
    //Load variabel editor
    blockSignals(true);
    for (int i = 0; i < colNames.size(); i++) {
        varNamesVE.push_back(QString(colNames[i]));
        QTableWidgetItem *item = new QTableWidgetItem;
        item->setText(QString(colNames[i]));
        setItem(i,0,item); //Set varname
     }
    setRExe(rExe);
    QStringList varTypes = rExe->getVarTypes();
    for (int i = 0; i < varTypes.size(); i++) {

        QTableWidgetItem *item = new QTableWidgetItem;
        item->setText(QString(varTypes[i]));
        setItem(i,2,item); //set vartype

        item = new QTableWidgetItem;
        if(varTypes[i] == "Real"){
            item->setText(QString::number(2));
        }else{
            item->setText(QString::number(0));
        }
        setItem(i,3,item); //set decimal
        item = new QTableWidgetItem;
        item->setText("Yes");
        setItem(i,4,item); //set missing
    }
    varTypesVE = varTypes;
    blockSignals(false);
    //connect(this, SIGNAL(cellChanged(int,int)),this,SLOT(changeVarProp(int, int)));
    //connect(this, SIGNAL(cellClicked(int,int)),this,SLOT(cellWidgetClicked(int,int)));
}

void VariableEditor::changeVarProp(int i, int j)
{
    //Saat ada perubahan di variabel editor juga mengubah data editor dan active dataset(kalau perlu)
    QString newVal;
    if(i<varNamesVE.size()){
        if(j == 0){
            //Mengganti nama variabel
            qDebug("j=0");
            varNamesVE.replace(i,item(i,j)->text());
            //set perubahan ke data editor dan active dataset
            dataEditor->setHorizontalHeaderItem(i,new QTableWidgetItem(QString(varNamesVE.at(i))));
            for(int row=0; row< activeDataset->rowCount(); row++){
                if(!activeDataset->verticalHeaderItem(row)->text().isEmpty()){
                    if(datasetName == activeDataset->item(row,2)->text()){
                        activeDataset->item(row+i+1,0)->setText(("\t"+QString(varNamesVE.at(i)))); //UBAH DATASET YG AKTIF
                    }
                }
            }
        }
        else if(j == 1){
            //Mengganti label
            newVal = item(i,j)->text();
            qDebug("ChangevarProp1 - " + newVal.toLatin1());
        }
        else if(j==2||j==4||j==5){
            //Mengganti col index 2,4,5
            qDebug("2/4/5 changed");
            if(cellWidget(i,j)){
                qDebug("ada widget");
                if(!((QComboBox*)cellWidget(i,j))->isHidden()){
                    newVal = ((QComboBox*)cellWidget(i,j))->currentText();
                    ((QComboBox*)cellWidget(i,j))->hide();
                    //item(i,j)->setText(newVal);
                }
            }
            if(j == 2){
                //Mengganti tipe variabel ke dataeditor
                qDebug("j=2");
                qDebug() << item(i,j)->text().toLocal8Bit();
                qDebug() << newVal.toLocal8Bit();
                qDebug() << varTypesVE[i].toLocal8Bit();

                if(newVal != varTypesVE.at(i) && newVal != ""){
                    QMessageBox::StandardButton changeVar;
                    changeVar = QMessageBox::question(this, item(i,0)->text()+" type changed", "Processed Changes?",
                                                    QMessageBox::Yes|QMessageBox::No);
                    if (changeVar == QMessageBox::No) {
                        qDebug("No");
                        return;
                    }

                    QTableWidgetItem *it = new QTableWidgetItem;
                    it->setText(newVal);
                    blockSignals(true);
                    setItem(i,j,it);
                    blockSignals(false);

                    varTypesVE.replace(i,item(i,j)->text());
                    dataEditor->changeVarType(i,varTypesVE.at(i));
                }
            }
        }
        else if(j==3){
            //Mengganti decimal
            if(!((QSpinBox*)cellWidget(i,j))->isHidden()){
                newVal = QString::number(((QSpinBox*)cellWidget(i,j))->value());
                ((QSpinBox*)cellWidget(i,j))->hide();
                QTableWidgetItem *item = new QTableWidgetItem;
                item->setText(newVal);
                setItem(i,j,item);
                //item(i,j)->setText(newVal);
            }
        }
        else{
            //Mengganti note
            newVal = item(i,j)->text();
            qDebug("ChangevarProp6b" + newVal.toLatin1());
        }
        qDebug("Setwindowsmodified");
        ((MainWindow*) QApplication::activeWindow())->setWindowModified(true);

    }else if(i == varNamesVE.size()){
        addNewVar(varNamesVE.size(),j);

        qDebug("Setwindowsmodified");
        ((MainWindow*) QApplication::activeWindow())->setWindowModified(true);
    }
    else{
        qDebug("TIDAK BOLEH - " + QString::number(i).toLatin1());
        removeRow(i);
        insertRow(i);
        checkWidgetVisibility();
    }
    setCurrentCell(i,j);
}

void VariableEditor::cellWidgetClicked(int i, int j)
{
    //Saat cell diklik muncul widget pada kolom tertentu, saat diklik di cell lain, widgetnya dihide
    qDebug("cellwidgetclicked");

    if(cellWidget(rowBefore,colBefore) && (colBefore ==2 || colBefore == 4 || colBefore ==5)){
        qDebug("ada widget");
        if(!((QComboBox*)cellWidget(rowBefore,colBefore))->isHidden()){
            ((QComboBox*)cellWidget(rowBefore,colBefore))->hide();
            QString newVal = ((QComboBox*)cellWidget(rowBefore,colBefore))->currentText();
            QTableWidgetItem *item = new QTableWidgetItem;
            item->setText(newVal);
            setItem(rowBefore,colBefore,item);
        }
    }else if(cellWidget(rowBefore,colBefore) && colBefore == 3){
        qDebug("ada widget");
        if(!((QSpinBox*)cellWidget(rowBefore,colBefore))->isHidden()){
            ((QSpinBox*)cellWidget(rowBefore,colBefore))->hide();
            QString newVal = QString::number(((QSpinBox*)cellWidget(rowBefore,colBefore))->value());
            QTableWidgetItem *item = new QTableWidgetItem;
            item->setText(newVal);
            setItem(rowBefore,colBefore,item);
        }
    }

    if(i<=varNamesVE.size() || (i==0 && varNamesVE.isEmpty())){
        qDebug("in");

        if(j==2 || j==4 || j==5){
            QSignalMapper* sMType = new QSignalMapper;

            QComboBox* combo = new QComboBox();

            //((QComboBox*)cellWidget(i,j))->show();
            if(j==2){
                combo->addItem("Real");
                combo->addItem("Integer");
                combo->addItem("String");

                QStringList varTypes = dataEditor->getVarTypes();
                if(varTypes.size()>0 && i < varTypes.size()){
                    if(varTypes[i] == "Real"){
                        combo->setCurrentIndex(0);
                    }else if(varTypes[i] == "Integer"){
                        combo->setCurrentIndex(1);
                    }else{
                        combo->setCurrentIndex(2);
                    }
                }
            }else if(j==4){
                combo->addItem("No");
                combo->addItem("Yes");
            }else{
                combo->addItem("Ordinal");
                combo->addItem("Nominal");
                combo->addItem("Interval");
                combo->addItem("Rasio");
            }
            setCellWidget(i,j,combo);

            connect(combo, SIGNAL(currentIndexChanged(int)),sMType, SLOT(map()));
            sMType->setMapping(combo, QString("%1").arg(i));
            connect(sMType, SIGNAL(mapped(const QString &)),this, SLOT(comboChanged(const QString &)));
        }
        else if(j==3){
            QSignalMapper* sMDecimal = new QSignalMapper;
            QSpinBox* spinDecimal = new QSpinBox();
            spinDecimal->setMaximum(10);
            setCellWidget(i,j,spinDecimal);

            connect(spinDecimal, SIGNAL(editingFinished()),sMDecimal, SLOT(map()));
            sMDecimal->setMapping(spinDecimal, QString("%1").arg(i));
            connect(sMDecimal, SIGNAL(mapped(const QString &)),this, SLOT(decimalChanged(const QString &)));
        }
    }
    rowBefore = i;
    colBefore = j;
}

void VariableEditor::comboChanged(const QString &indexString)
{
    changeVarProp(indexString.toInt(),currentColumn());
}

void VariableEditor::decimalChanged(const QString &indexString)
{
    changeVarProp(indexString.toInt(),3);
}

void VariableEditor::setRExe(RCore *rExe1)
{
    rExe = rExe1;
}

void VariableEditor::setDataEditor(DataEditor *dataEditor1)
{
    dataEditor = dataEditor1;
}

void VariableEditor::setActiveDataset(ActiveDataset *activeDataset1)
{
    activeDataset = activeDataset1;
}


VariableEditor::~VariableEditor()
{

}

void VariableEditor::reloadVar(QStringList varNames, QStringList varTypes)
{
    //Reload variabel editor, akan mengubah nama dan tipe variabel di data editor juga
    blockSignals(true);
    qDebug("reload varNames VE");
    if(varNames.size() != varTypes.size()){
        qDebug("SIZE TIDAK SAMA");
        return;
    }
    for (int i = 0; i < varNames.size(); i++) {
        QTableWidgetItem *item = new QTableWidgetItem;
        item->setText(varNames[i]);
        setItem(i,0,item);
        item = new QTableWidgetItem;
        item->setText(varTypes[i]);
        setItem(i,2,item);
    }
    varNamesVE = varNames;
    varTypesVE = varTypes;
    while(rowCount() > varNamesVE.size()){
        removeRow(varNamesVE.size());
    }
    setRowCount(20);
    dataEditor->setVarNames(varNamesVE);
    dataEditor->setVarTypes(varTypesVE);
    qDebug("------");
    blockSignals(false);
}

void VariableEditor::checkWidgetVisibility()
{
    //Chek apakah ada widget yg tampil, kalau ada hide widget tersebut, method ini dipanggil saat tab variabel editor dibuka
    qDebug("checkWidgetVisibility");
    for(int j=0; j< columnCount(); j++){
        for(int i=0; i<rowCount(); i++){
            if(cellWidget(i,j)){
                cellWidget(i,j)->hide();
            }
        }
    }
}

void VariableEditor::setDatasetName(QString name)
{
    //Saat set dataset yg aktif, seharusnya juga mengubah title di mainwindow, tapi masih error
    datasetName = name;
    //((MainWindow*) QApplication::activeWindow())->setCurrentFile(name);
}

void VariableEditor::deleteVarVE(int col)
{
    qDebug("delete VarVE");
    varTypesVE.erase(varTypesVE.begin()+col);
    varNamesVE.erase(varNamesVE.begin()+col);

    removeRow(col);

    //Update DataEditor
    dataEditor = activeDataset->getDataEditor();
    dataEditor->deleteVar(col);

    //Update Active Dataset
    QString datasetName = ((MainWindow*) QApplication::activeWindow())->windowTitle().split(".").at(0);
    int index = activeDataset->getDatasetIndex(datasetName);

    if(index != -1){
        QString path = activeDataset->getPathList().at(index);
        QString extension = activeDataset->getExtList().at(index);
        Rcpp::StringVector colNames;
        dataEditor = activeDataset->getDataEditor();
        for(int c=0; c<varNamesVE.size(); c++){
            colNames.push_back(varNamesVE[c].toStdString());
        }

        int rowIndex = activeDataset->getDatasetRowIndex(datasetName);
        if(rowIndex != -1){
            int rowCount = activeDataset->item(rowIndex,3)->text().split("and ").at(1).split(" Observation").at(0).toInt();
            int colCount = varNamesVE.size();
            activeDataset->updateRow(rowIndex,colNames,datasetName,datasetName,extension,path,colCount,rowCount);
        }
    }
}

void VariableEditor::showRowContextMenu(const QPoint& pos)
{
    qDebug("Click");
    int row = verticalHeader()->logicalIndexAt(pos);
    if(item(row,0)){ //Belum dicek kalau item text perbarisnya kosong(" ")
        QPoint globalPos = mapToGlobal(pos);
        QMenu myMenu;
        myMenu.addAction("Insert row");
        myMenu.addAction("Delete row");
        QAction* selectedItem = myMenu.exec(globalPos);
        if (selectedItem){
            if(selectedItem->text() == "Delete row"){
                qDebug("Delete row");
                deleteVarVE(row);
            }else if(selectedItem->text() == "Insert row"){
                qDebug("Insert row");
                addNewVarVE(row,"NewVar"+QString::number(row+1),"String");
            }
        }
    }
}
