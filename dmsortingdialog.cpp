#include "dmsortingdialog.h"
#include "ui_dmsortingdialog.h"
#include "mainwindow.h"

DMSortingDialog::DMSortingDialog(QWidget *parent, RInside &R) :
    QDialog(parent),
    ui(new Ui::DMSortingDialog),
    m_R(R)
{
    ui->setupUi(this);
    connect(ui->selectButton, SIGNAL(clicked()), this, SLOT(selectVar()));
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(sort()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(close()));
    ui->serNparRB->setChecked(true);
    //ui->groupBoxParallel->hide();
    //ui->datasetCB->hide();
    //ui->DatasetLabel->hide();
    //ui->datasetCB->setEnabled(false);
    connect(ui->datasetCB,SIGNAL(currentIndexChanged(int)),this,SLOT(selectDataset(int)));
}

DMSortingDialog::~DMSortingDialog()
{
    delete ui;
}

void DMSortingDialog::setDataDisplayedName(QString name)
{
    dataDisplayedName = name;
}

void DMSortingDialog::setDE(DataEditor *de)
{
    dataEditor = de;
}

void DMSortingDialog::setActiveDS(ActiveDataset *ads)
{
    activeDS = ads;
}

void DMSortingDialog::setSelectedRB1(const QString &idx){
    order.erase(order.begin()+idx.toInt());
    order.insert(order.begin()+idx.toInt(),true);
}

void DMSortingDialog::setSelectedRB2(const QString &idx){
    order.erase(order.begin()+idx.toInt());
    order.insert(order.begin()+idx.toInt(),false);
}

void DMSortingDialog::setCBDatasetName(QStringList list)
{
    ui->datasetCB->clear();
    ui->datasetCB->addItems(list);
}

void DMSortingDialog::setCurrentCBDSName(int index)
{
    ui->datasetCB->setCurrentIndex(index);
    selectDataset(index);

}

void DMSortingDialog::selectDataset(int index)
{
    if(ui->datasetCB->count() >0 ){
        Rcpp::CharacterVector varNames1 = m_R.parseEval("colnames("+ui->datasetCB->itemText(index).toStdString()+")");
        ui->listLeftVarWidget->clear();
        order.clear();
        varNames.clear();
        for(int i=0; i< varNames1.size(); i++){
            QListWidgetItem *item = new QListWidgetItem;
            item->setSizeHint(QSize(0,32));
            item->setText(QString(varNames1[i]));
            ui->listLeftVarWidget->addItem(item);

            order.push_back(true);
            varNames.push_back(QString(varNames1[i]));
        }
        if(ui->datasetCB->itemText(index) != dataDisplayedName){
            ui->loadDECheckBox->setEnabled(true);
        }else{
            ui->loadDECheckBox->setChecked(true);
            ui->loadDECheckBox->setEnabled(false);
        }
    }

}

void DMSortingDialog::selectVar()
{
    if(ui->listLeftVarWidget->currentRow()!=-1){
        //addRB
        QList<QListWidgetItem*> l = ui->listLeftVarWidget->selectedItems();
        if (! l.empty()) {
            for(int i=0;i<l.length();i++){
                int idx;
                for(int j=0; j< varNames.size(); j++){
                    if(l.at(i)->text() == varNames[j]){
                        idx = j;
                    }
                }
                QGroupBox *groupBox = new QGroupBox;

                QLabel *label = new QLabel;
                label->setText(l.at(i)->text());
                QRadioButton *radio1 = new QRadioButton(tr("&Ascending"));
                QRadioButton *radio2 = new QRadioButton(tr("&Descending"));

                radio1->setChecked(true);

                QHBoxLayout *hLayout = new QHBoxLayout;
                hLayout->addWidget(label);
                hLayout->addWidget(radio1);
                hLayout->addWidget(radio2);
                groupBox->setLayout(hLayout);
                groupBox->setStyleSheet("border:0;");

                QListWidgetItem *item = new QListWidgetItem;
                item->setSizeHint(QSize(0,32));
                ui->orderListWidget->addItem(item);
                ui->orderListWidget->setItemWidget(item,groupBox);
                QSignalMapper *sM = new QSignalMapper;
                connect(radio1,SIGNAL(clicked()),sM,SLOT(map()));
                sM->setMapping(radio1, QString("%1").arg(idx));
                connect(sM,SIGNAL(mapped(QString)),this,SLOT(setSelectedRB1(QString)));

                sM = new QSignalMapper;
                connect(radio2,SIGNAL(clicked()),sM,SLOT(map()));
                sM->setMapping(radio2, QString("%1").arg(idx));
                connect(sM,SIGNAL(mapped(QString)),this,SLOT(setSelectedRB2(QString)));

            }
        }
        moveCurrentItem(ui->listLeftVarWidget, ui->listRightVarWidget);
    }else{
        //removeRB
        int idx;
        QList<QListWidgetItem*> l = ui->listRightVarWidget->selectedItems();
        if (! l.empty()) {
            for(int i=0; i< ui->listRightVarWidget->count(); i++){
                for(int j=0;j<l.length();j++){
                    if(ui->listRightVarWidget->item(i)->text() == l.at(j)->text()){
                        idx = i;
                        break;
                    }
                }
            }
            delete ui->orderListWidget->item(idx);
        }
        moveCurrentItem(ui->listRightVarWidget, ui->listLeftVarWidget);
    }
}

void DMSortingDialog::moveCurrentItem(QListWidget *source, QListWidget *target)
{
    QList<QListWidgetItem*> l = source->selectedItems();
    source->setCurrentRow(-1);
    if (! l.empty()) {
        for(int i=0;i<l.length();i++){
            QListWidgetItem *item = new QListWidgetItem;
            item->setSizeHint(QSize(0,32));
            item->setText(l.at(i)->text());
            target->addItem(item);
            delete l.at(i);
        }
        l.clear();
    }
}

void DMSortingDialog::sort()
{
    qDebug("------- Sorting "+ ui->datasetCB->currentText().toLocal8Bit() +"---------");
    //QProgressDialog progress("Sorting...","Cancel" , 0, 100, this);
    //progress.setWindowModality(Qt::WindowModal);
    //progress.setWindowFlags(Qt::SplashScreen);
    //QList<QPushButton *> L=progress.findChildren<QPushButton *>();
    //L.at(0)->hide();
    if(ui->listRightVarWidget->count()<1){
        return;
    }
    DataManagement *dm = new DataManagement;
    //double cno = omp_get_wtime();
    std::vector<int> cols, cols1;
    std::vector<bool> order1;

    //double tc1 = omp_get_wtime();
    std::vector<std::vector<type> > VM;
    Rcpp::DataFrame dataFrame;
    if(ui->datasetCB->currentText() == dataDisplayedName){
        //qDebug("Samaaaaaaa");
        VM = dataEditor->getVM();
        varTypes = dataEditor->getVarTypes();
    }else{
        //qDebug("Bedaaaaaaaa");
        dataFrame = m_R.parseEval(ui->datasetCB->currentText().toStdString());
        varTypes = dm->getVarTypesFromDF(dataFrame);
        Rcpp::CharacterVector charVector = dataFrame[0];
        VM.resize(charVector.size(), std::vector<type>(dataFrame.size()));
        for(int j=0;j<dataFrame.size();j++)
        {
            if(varTypes[j]=="String"){
                charVector = dataFrame[j];
                for(int i=0;i<charVector.size();i++){
                    VM[i][j].string = charVector[i];
                }
            }else{
                Rcpp::NumericVector numVector = dataFrame[j];
                for(int i=0;i<numVector.size();i++){
                    VM[i][j].real = numVector[i];
                }
            }
        }
    }
    //tc1 = omp_get_wtime()-tc1;
    //qDebug("get VM n vartypes");
    //qDebug() << tc1;

    QStringList types;
    for(int i=0; i<ui->listRightVarWidget->count(); i++){
        int c;
        for(int j = 0; j< varNames.size(); j++){
            if(ui->listRightVarWidget->item(i)->text() == varNames[j]){
                c = j;
            }
        }
        cols.push_back(c);
        order1.push_back(order[c]);
        types.push_back(varTypes[c]);
    }

    //double tc2;
    std::vector<std::vector<type> > VM1;
    if(cols.size() < VM[0].size()){
        //tc2 = omp_get_wtime();
        int colCount = cols.size()+1;
        VM1.resize(VM.size(), std::vector<type>(colCount));
        for(unsigned i=0;i<VM1.size();i++){
            VM1[i][0].real = i; //add row index
        }
        types.clear();
        types.push_back("Integer"); //add row index
        for(unsigned j=1;j<VM1[0].size();j++)
        {
            int c = cols[j-1];
            if(varTypes.at(c)=="String"){
                for(unsigned i=0;i<VM.size();i++){
                    VM1[i][j].string = VM[i][c].string;
                }
                types.push_back("String");
            }else{
                for(unsigned i=0;i<VM1.size();i++){
                    VM1[i][j].real = VM[i][c].real;
                }
                types.push_back("Real");
            }
            cols1.push_back(j);
        }
        //tc2 = omp_get_wtime() - tc2;
        //qDebug("Copy selected column");
        //qDebug() << tc2;
    }

    //double ts, tp2;
    if(ui->serNparRB->isChecked()){
        qDebug("-------------------Serial----");
        //ts = omp_get_wtime();
        if(cols.size() < VM[0].size()){
            dm->mergeSort(VM1,cols1,order1,types,1);
        }else{
            dm->mergeSort(VM,cols,order1,types,1);
        }
        //ts = omp_get_wtime() -ts;
        //qDebug("Total serial");
        //ts = ts + tc2;
        //qDebug() << ts;

        if(cols.size() < VM[0].size()){ //reset value VM1
            for(unsigned i=0;i<VM1.size();i++){
                VM1[i][0].real = i;
            }
            for(unsigned j=1;j<VM1[0].size();j++)
            {
                int c = cols[j-1];
                if(varTypes.at(c)=="String"){
                    for(unsigned i=0;i<VM1.size();i++){
                        VM1[i][j].string = VM[i][c].string;
                    }
                }else{
                    for(unsigned i=0;i<VM1.size();i++){
                        VM1[i][j].real = VM[i][c].real;
                    }
                }
            }
        }

        /*qDebug("-------------------2 Core----");
        tp2 = omp_get_wtime();
        if(cols.size() < VM[0].size()){
            dm->mergeSort(VM1,cols1,order1,types,2);
        }else{
            dm->mergeSort(VM,cols,order1,types,2);
        }
        tp2 = omp_get_wtime() -tp2;
        tp2 = tp2 + tc2;
        qDebug("Total Par 2");
        qDebug() << tp2;

        if(cols.size() < VM[0].size()){ //reset value VM1
            for(unsigned i=0;i<VM1.size();i++){
                VM1[i][0].real = i;
            }
            for(unsigned j=1;j<VM1[0].size();j++)
            {
                int c = cols[j-1];
                if(varTypes.at(c)=="String"){
                    for(unsigned i=0;i<VM1.size();i++){
                        VM1[i][j].string = VM[i][c].string;
                    }
                }else{
                    for(unsigned i=0;i<VM1.size();i++){
                        VM1[i][j].real = VM[i][c].real;
                    }
                }
            }
        }*/
    }
    /*
    qDebug("-------------------4 Core----");
    double tp4 = omp_get_wtime();
    if(cols.size() < VM[0].size()){
        dm->mergeSort(VM1,cols1,order1,types,4);
    }else{
        dm->mergeSort(VM,cols,order1,types,4);
    }
    tp4 = omp_get_wtime() -tp4;
    tp4 = tp4 + tc2;
    qDebug("Total Par4");
    qDebug() << tp4;

    delete dm;
    qDebug("=============");
    if(ui->serNparRB->isChecked()){
        qDebug("Speedup");
        qDebug("Par2");
        qDebug() << ts/(tp2);
        qDebug("Par4");
        qDebug() << ts/(tp4);
        qDebug("Eff");
        qDebug("Par2");
        qDebug() << ts/(2*(tp2));
        qDebug("Par4");
        qDebug() << ts/(4*(tp4));
    }
    */
    if(ui->loadDECheckBox->isChecked()){
        if(dataEditor->isHidden()){
            setVtoDF:
            //Vector to DataFrame
            //tc2 = omp_get_wtime();
            for(unsigned j =0; j< VM[0].size();j++ ){
                if(varTypes.at(j)=="String"){
                    std::vector<std::string> V;
                    for(unsigned i=0; i< VM.size(); i++){
                        int index = VM1[i][0].real;
                        V.push_back(VM[index][j].string);
                    }
                    dataFrame[j] = V;
                }
                else if(varTypes.at(j)=="Integer"){
                    std::vector<int> Vi;
                    for(unsigned i=0; i< VM.size(); i++){
                        int index = VM1[i][0].real;
                        Vi.push_back(VM[index][j].real);
                    }
                    dataFrame[j] = Vi;
                }else{
                    std::vector<double> Vd;
                    for(unsigned i=0; i< VM.size(); i++){
                        int index = VM1[i][0].real;
                        Vd.push_back(VM[index][j].real);
                    }
                    dataFrame[j] = Vd;
                }
            }
            m_R[ui->datasetCB->currentText().toStdString()] = dataFrame;
            //tc2 = omp_get_wtime()-tc2;
            //qDebug("V to DF");
            //qDebug() << tc2;
            if(ui->loadDECheckBox->isChecked()){
                activeDS->reloadData(ui->datasetCB->currentText());
            }else{
                goto finishedNotLoad;
            }
        }else{
            if(ui->datasetCB->currentText() == dataDisplayedName){
                //tc2 = omp_get_wtime();
                if(cols.size() < VM[0].size()){
                    dataEditor->setVMsorted(VM1);
                    dataEditor->reloadTable(false,true);
                }else{
                    dataEditor->setVM(VM);
                    dataEditor->reloadTable(false,false);
                }
                //tc2 = omp_get_wtime()-tc2;
                //qDebug("Reload tabel");
                //qDebug() << tc2;
            }
            else{
                qDebug("Diff Dataset");
                //tc2 = omp_get_wtime();
                dataEditor->setVarTypes(varTypes);
                dataEditor->setVarNames(varNames);
                if(cols.size() < VM[0].size()){
                    dataEditor->setVMsorted(VM1);
                    dataEditor->setVM(VM);
                    dataEditor->reloadTable(true,true);
                }else{
                    dataEditor->setVM(VM);
                    dataEditor->reloadTable(true,false);
                }
                //tc2 = omp_get_wtime()-tc2;
                //qDebug("Reload tabel");
                //qDebug() << tc2;

                MainWindow *mw = (MainWindow*) QApplication::activeWindow();
                QStringList dataset = activeDS->getDatasetNameList();
                QString ext;
                for(int i=0; i<dataset.size();i++){
                    if(dataset[i] == ui->datasetCB->currentText()){
                        ext = activeDS->getExtList().at(i);
                    }
                }
                mw->setCurrentFile(ui->datasetCB->currentText()+"."+ext);
            }
        }
    }else{
        goto setVtoDF;
        finishedNotLoad:
        QMessageBox::information(this,"Info","Sorting finished");
    }
}

