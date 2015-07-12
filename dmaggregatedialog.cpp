#include "dmaggregatedialog.h"
#include "ui_dmaggregatedialog.h"
#include "mainwindow.h"

DMAggregateDialog::DMAggregateDialog(QWidget *parent, RInside &R) :
    QDialog(parent),
    ui(new Ui::DMAggregateDialog),
    m_R(R)
{
    ui->setupUi(this);
    connect(ui->selectIDButton, SIGNAL(clicked()), this, SLOT(selectVarID()));
    connect(ui->selectTargetButton, SIGNAL(clicked()), this, SLOT(selectVarTarget()));
    connect(this, SIGNAL(accepted()), this, SLOT(aggregate()));
    ui->serNparRB->setChecked(true);

    QStringList function;
    function.push_back("SUM");
    function.push_back("MEAN");
    function.push_back("MEDIAN");
    function.push_back("COUNT");
    function.push_back("FIRST");
    function.push_back("LAST");
    function.push_back("MIN");
    function.push_back("MAX");
    function.push_back("PERCENTAGE ABOVE");
    function.push_back("PERCENTAGE BELOW");
    function.push_back("PERCENTAGE INSIDE");
    function.push_back("PERCENTAGE OUTSIDE");
    ui->aggFunctCB->addItems(function);
    ui->newDS_RB->setChecked(true);
}

DMAggregateDialog::~DMAggregateDialog()
{
    delete ui;
}


void DMAggregateDialog::setDataDisplayedName(QString name)
{
    dataDisplayedName = name;
}

void DMAggregateDialog::setDE(DataEditor *de)
{
    dataEditor = de;
}

void DMAggregateDialog::setCBDatasetName(QStringList list)
{
    ui->datasetCB->clear();
    ui->datasetCB->addItems(list);
}

void DMAggregateDialog::setCurrentCBDSName(int index)
{
    ui->datasetCB->setCurrentIndex(index);
    selectDataset(index);
    connect(ui->datasetCB,SIGNAL(currentIndexChanged(int)),this,SLOT(selectDataset(int)));
}

void DMAggregateDialog::selectDataset(int index)
{
    Rcpp::CharacterVector varNames1 = m_R.parseEval("colnames("+ui->datasetCB->itemText(index).toStdString()+")");
    ui->listLeftVarWidget->clear();
    varNames.clear();
    for(int i=0; i< varNames1.size(); i++){
        QListWidgetItem *item = new QListWidgetItem;
        item->setSizeHint(QSize(0,32));
        item->setText(QString(varNames1[i]));
        ui->listLeftVarWidget->addItem(item);

        varNames.push_back(QString(varNames1[i]));
    }
}

void DMAggregateDialog::selectVarID()
{
    if(ui->listLeftVarWidget->currentRow()!=-1){
        moveCurrentItem(ui->listLeftVarWidget, ui->colIDListVarWidget);
    }else{
        moveCurrentItem(ui->colIDListVarWidget, ui->listLeftVarWidget);
    }

    if(ui->colIDListVarWidget->count()>0){
        ui->aggFunctCB->setEnabled(true);
    }else{
        ui->aggFunctCB->setEnabled(false);
    }
}

void DMAggregateDialog::selectVarTarget()
{
    if(ui->listLeftVarWidget->currentRow()!=-1){
        moveCurrentItem(ui->listLeftVarWidget, ui->colTargetListVarWidget);
    }else{
        moveCurrentItem(ui->colTargetListVarWidget, ui->listLeftVarWidget);
    }
}

void DMAggregateDialog::moveCurrentItem(QListWidget *source, QListWidget *target)
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

void DMAggregateDialog::setActiveDS(ActiveDataset *ads1)
{
    activeDS = ads1;
}

void DMAggregateDialog::aggregate()
{
    if(ui->colIDListVarWidget->count()==0){
        return;
    }
    /*
    qDebug("------ Aggregate "+ ui->datasetCB->currentText().toLocal8Bit() +"-------");
    QString sFunction = ui->aggFunctCB->currentText();
    double above, below, inside1, inside2, outside1, outside2;
    if(sFunction == "PERCENTAGE ABOVE"){
        above = QInputDialog::getDouble(this, "Percentage Above","Above");
    }else if(sFunction == "PERCENTAGE BELOW"){
        below = QInputDialog::getDouble(this, "Percentage below","Below");
    }else if(sFunction == "PERCENTAGE INSIDE"){
        inside1 = QInputDialog::getDouble(this, "Percentage Inside","Inside low");
        inside2 = QInputDialog::getDouble(this, "Percentage Inside","Inside high");
    }else if(sFunction == "PERCENTAGE OUTSIDE"){
        outside1 = QInputDialog::getDouble(this, "Percentage Inside","Outside low");
        outside2 = QInputDialog::getDouble(this, "Percentage Inside","Outside high");
    }

    DataManagement *dm = new DataManagement;

    double tc1 = omp_get_wtime();
    std::vector<std::vector<type> > VM;
    Rcpp::DataFrame dataFrame;
    Rcpp::CharacterVector charVector;
    if(ui->datasetCB->currentText() == dataDisplayedName){
        VM = dataEditor->getVM();
        varTypes = dataEditor->getVarTypes();
    }else{
        dataFrame = m_R.parseEval(ui->datasetCB->currentText().toStdString());
        varTypes = dm->getVarTypesFromDF(dataFrame);
        charVector = dataFrame[0];
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
    tc1 = omp_get_wtime()-tc1;
    qDebug("get VM n vartypes");
    qDebug() << tc1;

    std::vector<int> colID;
    std::vector<bool> order;
    QStringList types;
    for(int i=0; i< varNames.size(); i++){
        for(int j=0; j< ui->colIDListVarWidget->count(); j++){
            if(varNames[i] == ui->colIDListVarWidget->item(j)->text()){
                colID.push_back(i);
                order.push_back(true);
                types.push_back(varTypes[i]);
            }
        }
    }
    std::vector<int> colTarget;
    for(int i=0; i< varNames.size(); i++){
        for(int j=0; j< ui->colTargetListVarWidget->count(); j++){
            if(varNames[i] == ui->colTargetListVarWidget->item(j)->text()){
                colTarget.push_back(i);
            }
        }
    }
    std::vector<std::vector<std::vector<type> > > VMAgg, VMAgg1, VMAgg2, VMAgg3, VMAgg4;
    double ts, tp2, tp4;
    if(ui->serNparRB->isChecked()){
        //-------------------------------------------1 Core
        ts = omp_get_wtime();
        //VM = mergesort(VM, 1, colID, order);
        dm->mergeSort(VM,colID,order,types,1);
        qDebug("Sort Ser");
        qDebug() << omp_get_wtime() - ts;
        double ts1 = omp_get_wtime();
        VMAgg = aggregate1(VM,0,VM.size(), colID,colTarget);
        ts1 = omp_get_wtime() - ts1;
        qDebug("Serial");
        qDebug()<<ts1;
        ts = omp_get_wtime() - ts;

        //-----------------------------
        if(ui->datasetCB->currentText() == dataDisplayedName){
            VM = dataEditor->getVM();
            varTypes = dataEditor->getVarTypes();
        }else{
            dataFrame = m_R.parseEval(ui->datasetCB->currentText().toStdString());
            varTypes = dm->getVarTypesFromDF(dataFrame);
            charVector = dataFrame[0];
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
        //-------------------------------------------2 Core
        int low1 = 0, high1 = VM.size()/2, low2 = high1, high2 = VM.size();
        tp2 = omp_get_wtime();
        //VM = mergesort(VM, 2, colID, order);
        dm->mergeSort(VM,colID,order,types,2);
        qDebug("Sort 2 Core");
        qDebug() << omp_get_wtime() - tp2;
        //int colID = k;
        double tp2a = omp_get_wtime();
        #pragma omp parallel sections
        {
            #pragma omp section
            {
                //1
                std::vector<std::vector<type> > VColID, VColTarget;
                std::vector<type> V;
                type t;
                bool same = false;
                for(unsigned c=0; c< colID.size(); c++){
                    if(varTypes.at(colID[c])!="String"){
                        t.real = VM[low1][colID[c]].real;
                        V.push_back(t);
                    }else{
                        t.string = VM[low1][colID[c]].string;
                        V.push_back(t);
                    }
                }
                VColID.push_back(V);
                VMAgg1.push_back(VColID);

                for(unsigned ct=0; ct< colTarget.size(); ct++){
                    V.clear();
                    VColTarget.clear();

                    t.real = VM[low1][colTarget[ct]].real;
                    V.push_back(t);
                    VColTarget.push_back(V);
                    VMAgg1.push_back(VColTarget);
                }

                for(int i=low1+1; i<high1; i++){
                    for(unsigned c=0; c< colID.size(); c++){
                        if(varTypes.at(colID[c])!="String"){
                            if(VM[i][colID[c]].real != VM[i-1][colID[c]].real){
                                same = false;
                                break;
                            }else{
                                same = true;
                            }
                        }else{
                            if(VM[i][colID[c]].string != VM[i-1][colID[c]].string){
                                same = false;
                                break;
                            }else{
                                same = true;
                            }
                        }
                    }
                    if(same){
                        //QMessageBox::information(0,QString::number(i),"same");
                        for(unsigned ct=1; ct < VMAgg1.size(); ct++){
                            t.real = VM[i][colTarget[ct-1]].real;
                            VMAgg1[ct][VMAgg1[ct].size()-1].push_back(t);
                            //QMessageBox::information(0,QString::number(i),QString::number(t.real));
                        }
                    }else{
                        //QMessageBox::information(0,QString::number(i),"not same");
                        V.clear();
                        for(unsigned c=0; c< colID.size(); c++){
                            if(varTypes.at(colID[c])!="String"){
                                t.real = VM[i][colID[c]].real;
                                V.push_back(t);
                            }else{
                                t.string = VM[i][colID[c]].string;
                                V.push_back(t);
                            }
                        }
                        VMAgg1[0].push_back(V);

                        for(unsigned ct=1; ct< VMAgg1.size(); ct++){
                            V.clear();
                            t.real = VM[i][colTarget[ct-1]].real;
                            V.push_back(t);
                            VMAgg1[ct].push_back(V);
                            //QMessageBox::information(0,QString::number(i),QString::number(t.real));
                        }
                    }
                }
            }
            #pragma omp section
            {
                //2
                std::vector<std::vector<type> > VColID, VColTarget;
                std::vector<type> V;
                type t;
                bool same = false;
                for(unsigned c=0; c< colID.size(); c++){
                    if(varTypes.at(colID[c])!="String"){
                        t.real = VM[low2][colID[c]].real;
                        V.push_back(t);
                    }else{
                        t.string = VM[low2][colID[c]].string;
                        V.push_back(t);
                    }
                }
                VColID.push_back(V);
                VMAgg2.push_back(VColID);

                for(unsigned ct=0; ct< colTarget.size(); ct++){
                    V.clear();
                    VColTarget.clear();

                    t.real = VM[low2][colTarget[ct]].real;
                    V.push_back(t);
                    VColTarget.push_back(V);
                    VMAgg2.push_back(VColTarget);
                }

                for(int i=low2+1; i<high2; i++){
                    for(unsigned c=0; c< colID.size(); c++){
                        if(varTypes.at(colID[c])!="String"){
                            if(VM[i][colID[c]].real != VM[i-1][colID[c]].real){
                                same = false;
                                break;
                            }else{
                                same = true;
                            }
                        }else{
                            if(VM[i][colID[c]].string != VM[i-1][colID[c]].string){
                                same = false;
                                break;
                            }else{
                                same = true;
                            }
                        }
                    }
                    if(same){
                        //QMessageBox::information(0,QString::number(i),"same");
                        for(unsigned ct=1; ct < VMAgg2.size(); ct++){
                            t.real = VM[i][colTarget[ct-1]].real;
                            VMAgg2[ct][VMAgg2[ct].size()-1].push_back(t);
                            //QMessageBox::information(0,QString::number(i),QString::number(t.real));
                        }
                    }else{
                        //QMessageBox::information(0,QString::number(i),"not same");
                        V.clear();
                        for(unsigned c=0; c< colID.size(); c++){
                            if(varTypes.at(colID[c])!="String"){
                                t.real = VM[i][colID[c]].real;
                                V.push_back(t);
                            }else{
                                t.string = VM[i][colID[c]].string;
                                V.push_back(t);
                            }
                        }
                        VMAgg2[0].push_back(V);

                        for(unsigned ct=1; ct< VMAgg2.size(); ct++){
                            V.clear();
                            t.real = VM[i][colTarget[ct-1]].real;
                            V.push_back(t);
                            VMAgg2[ct].push_back(V);
                            //QMessageBox::information(0,QString::number(i),QString::number(t.real));
                        }
                    }
                }
            }
        }

        bool same12 = false;
        for(unsigned i=0; i<VMAgg2[0].size(); i++){
            for(unsigned j=0; j<VMAgg2[0][i].size(); j++){
                //QMessageBox::information(0,"",QString::number(VMAgg1[0][VMAgg1[0].size()-1][j].real)+"-->"+QString::number(VMAgg3[0][i][j].real));
                if(VMAgg1[0][VMAgg1[0].size()-1][j].real == VMAgg2[0][i][j].real){
                    same12 = true;
                }else{
                    same12 = false;
                    break;
                }
            }
            if(same12){
                for(unsigned ct = 1; ct< VMAgg1.size(); ct++){
                    for(unsigned j=0; j<VMAgg2[ct][i].size(); j++){
                        //QMessageBox::information(0,"same", QString::number(VMAgg3[ct][i][j].real));
                        VMAgg1[ct][VMAgg1[ct].size()-1].push_back(VMAgg2[ct][i][j]);
                    }
                }

            }else{
                //QMessageBox::information(0,"not same","");
                VMAgg1[0].push_back(VMAgg2[0][i]);
                for(unsigned ct = 1; ct< VMAgg1.size(); ct++){
                    VMAgg1[ct].push_back(VMAgg2[ct][i]);
                }
            }
        }
        tp2a = omp_get_wtime() - tp2a;
        qDebug("Parallel 2");
        qDebug()<<tp2a;
        tp2 = omp_get_wtime()-tp2;

        //-----------------------------
        if(ui->datasetCB->currentText() == dataDisplayedName){
            VM = dataEditor->getVM();
            varTypes = dataEditor->getVarTypes();
        }else{
            dataFrame = m_R.parseEval(ui->datasetCB->currentText().toStdString());
            varTypes = dm->getVarTypesFromDF(dataFrame);
            charVector = dataFrame[0];
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
    }
    //-------------------------------------------4 Core
    VMAgg1.clear();
    VMAgg2.clear();
    int low1 = 0, high1 = VM.size()/4, low2 = high1, high2 = VM.size()/2, low3=high2, high3 = VM.size()*3/4, low4 = high3, high4 = VM.size();
    tp4 = omp_get_wtime();
    //VM = mergesort(VM, 4, colID, order);
    dm->mergeSort(VM,colID,order,types,4);
    qDebug("Sort 4 Core");
    qDebug() << omp_get_wtime() - tp4;
    //int colID = k;
    double tp4a = omp_get_wtime();
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            //1
            std::vector<std::vector<type> > VColID, VColTarget;
            std::vector<type> V;
            type t;
            bool same = false;
            for(unsigned c=0; c< colID.size(); c++){
                if(varTypes.at(colID[c])!="String"){
                    t.real = VM[low1][colID[c]].real;
                    V.push_back(t);
                }else{
                    t.string = VM[low1][colID[c]].string;
                    V.push_back(t);
                }
            }
            VColID.push_back(V);
            VMAgg1.push_back(VColID);

            for(unsigned ct=0; ct< colTarget.size(); ct++){
                V.clear();
                VColTarget.clear();

                t.real = VM[low1][colTarget[ct]].real;
                V.push_back(t);
                VColTarget.push_back(V);
                VMAgg1.push_back(VColTarget);
            }

            for(int i=low1+1; i<high1; i++){
                for(unsigned c=0; c< colID.size(); c++){
                    if(varTypes.at(colID[c])!="String"){
                        if(VM[i][colID[c]].real != VM[i-1][colID[c]].real){
                            same = false;
                            break;
                        }else{
                            same = true;
                        }
                    }else{
                        if(VM[i][colID[c]].string != VM[i-1][colID[c]].string){
                            same = false;
                            break;
                        }else{
                            same = true;
                        }
                    }
                }
                if(same){
                    //QMessageBox::information(0,QString::number(i),"same");
                    for(unsigned ct=1; ct < VMAgg1.size(); ct++){
                        t.real = VM[i][colTarget[ct-1]].real;
                        VMAgg1[ct][VMAgg1[ct].size()-1].push_back(t);
                        //QMessageBox::information(0,QString::number(i),QString::number(t.real));
                    }
                }else{
                    //QMessageBox::information(0,QString::number(i),"not same");
                    V.clear();
                    for(unsigned c=0; c< colID.size(); c++){
                        if(varTypes.at(colID[c])!="String"){
                            t.real = VM[i][colID[c]].real;
                            V.push_back(t);
                        }else{
                            t.string = VM[i][colID[c]].string;
                            V.push_back(t);
                        }
                    }
                    VMAgg1[0].push_back(V);

                    for(unsigned ct=1; ct< VMAgg1.size(); ct++){
                        V.clear();
                        t.real = VM[i][colTarget[ct-1]].real;
                        V.push_back(t);
                        VMAgg1[ct].push_back(V);
                        //QMessageBox::information(0,QString::number(i),QString::number(t.real));
                    }
                }
            }
        }
        #pragma omp section
        {
            //2
            std::vector<std::vector<type> > VColID, VColTarget;
            std::vector<type> V;
            type t;
            bool same = false;
            for(unsigned c=0; c< colID.size(); c++){
                if(varTypes.at(colID[c])!="String"){
                    t.real = VM[low2][colID[c]].real;
                    V.push_back(t);
                }else{
                    t.string = VM[low2][colID[c]].string;
                    V.push_back(t);
                }
            }
            VColID.push_back(V);
            VMAgg2.push_back(VColID);

            for(unsigned ct=0; ct< colTarget.size(); ct++){
                V.clear();
                VColTarget.clear();

                t.real = VM[low2][colTarget[ct]].real;
                V.push_back(t);
                VColTarget.push_back(V);
                VMAgg2.push_back(VColTarget);
            }

            for(int i=low2+1; i<high2; i++){
                for(unsigned c=0; c< colID.size(); c++){
                    if(varTypes.at(colID[c])!="String"){
                        if(VM[i][colID[c]].real != VM[i-1][colID[c]].real){
                            same = false;
                            break;
                        }else{
                            same = true;
                        }
                    }else{
                        if(VM[i][colID[c]].string != VM[i-1][colID[c]].string){
                            same = false;
                            break;
                        }else{
                            same = true;
                        }
                    }
                }
                if(same){
                    //QMessageBox::information(0,QString::number(i),"same");
                    for(unsigned ct=1; ct < VMAgg2.size(); ct++){
                        t.real = VM[i][colTarget[ct-1]].real;
                        VMAgg2[ct][VMAgg2[ct].size()-1].push_back(t);
                        //QMessageBox::information(0,QString::number(i),QString::number(t.real));
                    }
                }else{
                    //QMessageBox::information(0,QString::number(i),"not same");
                    V.clear();
                    for(unsigned c=0; c< colID.size(); c++){
                        if(varTypes.at(colID[c])!="String"){
                            t.real = VM[i][colID[c]].real;
                            V.push_back(t);
                        }else{
                            t.string = VM[i][colID[c]].string;
                            V.push_back(t);
                        }
                    }
                    VMAgg2[0].push_back(V);

                    for(unsigned ct=1; ct< VMAgg2.size(); ct++){
                        V.clear();
                        t.real = VM[i][colTarget[ct-1]].real;
                        V.push_back(t);
                        VMAgg2[ct].push_back(V);
                        //QMessageBox::information(0,QString::number(i),QString::number(t.real));
                    }
                }
            }
        }
        #pragma omp section
        {
            //3
            std::vector<std::vector<type> > VColID, VColTarget;
            std::vector<type> V;
            type t;
            bool same = false;
            for(unsigned c=0; c< colID.size(); c++){
                if(varTypes.at(colID[c])!="String"){
                    t.real = VM[low3][colID[c]].real;
                    V.push_back(t);
                }else{
                    t.string = VM[low3][colID[c]].string;
                    V.push_back(t);
                }
            }
            VColID.push_back(V);
            VMAgg3.push_back(VColID);

            for(unsigned ct=0; ct< colTarget.size(); ct++){
                V.clear();
                VColTarget.clear();

                t.real = VM[low3][colTarget[ct]].real;
                V.push_back(t);
                VColTarget.push_back(V);
                VMAgg3.push_back(VColTarget);
            }

            for(int i=low3+1; i<high3; i++){
                for(unsigned c=0; c< colID.size(); c++){
                    if(varTypes.at(colID[c])!="String"){
                        if(VM[i][colID[c]].real != VM[i-1][colID[c]].real){
                            same = false;
                            break;
                        }else{
                            same = true;
                        }
                    }else{
                        if(VM[i][colID[c]].string != VM[i-1][colID[c]].string){
                            same = false;
                            break;
                        }else{
                            same = true;
                        }
                    }
                }
                if(same){
                    //QMessageBox::information(0,QString::number(i),"same");
                    for(unsigned ct=1; ct < VMAgg3.size(); ct++){
                        t.real = VM[i][colTarget[ct-1]].real;
                        VMAgg3[ct][VMAgg3[ct].size()-1].push_back(t);
                        //QMessageBox::information(0,QString::number(i),QString::number(t.real));
                    }
                }else{
                    //QMessageBox::information(0,QString::number(i),"not same");
                    V.clear();
                    for(unsigned c=0; c< colID.size(); c++){
                        if(varTypes.at(colID[c])!="String"){
                            t.real = VM[i][colID[c]].real;
                            V.push_back(t);
                        }else{
                            t.string = VM[i][colID[c]].string;
                            V.push_back(t);
                        }
                    }
                    VMAgg3[0].push_back(V);

                    for(unsigned ct=1; ct< VMAgg3.size(); ct++){
                        V.clear();
                        t.real = VM[i][colTarget[ct-1]].real;
                        V.push_back(t);
                        VMAgg3[ct].push_back(V);
                        //QMessageBox::information(0,QString::number(i),QString::number(t.real));
                    }
                }
            }
        }
        #pragma omp section
        {
            //4
            std::vector<std::vector<type> > VColID, VColTarget;
            std::vector<type> V;
            type t;
            bool same = false;
            for(unsigned c=0; c< colID.size(); c++){
                if(varTypes.at(colID[c])!="String"){
                    t.real = VM[low4][colID[c]].real;
                    V.push_back(t);
                }else{
                    t.string = VM[low4][colID[c]].string;
                    V.push_back(t);
                }
            }
            VColID.push_back(V);
            VMAgg4.push_back(VColID);

            for(unsigned ct=0; ct< colTarget.size(); ct++){
                V.clear();
                VColTarget.clear();

                t.real = VM[low4][colTarget[ct]].real;
                V.push_back(t);
                VColTarget.push_back(V);
                VMAgg4.push_back(VColTarget);
            }

            for(int i=low4+1; i<high4; i++){
                for(unsigned c=0; c< colID.size(); c++){
                    if(varTypes.at(colID[c])!="String"){
                        if(VM[i][colID[c]].real != VM[i-1][colID[c]].real){
                            same = false;
                            break;
                        }else{
                            same = true;
                        }
                    }else{
                        if(VM[i][colID[c]].string != VM[i-1][colID[c]].string){
                            same = false;
                            break;
                        }else{
                            same = true;
                        }
                    }
                }
                if(same){
                    //QMessageBox::information(0,QString::number(i),"same");
                    for(unsigned ct=1; ct < VMAgg4.size(); ct++){
                        t.real = VM[i][colTarget[ct-1]].real;
                        VMAgg4[ct][VMAgg4[ct].size()-1].push_back(t);
                        //QMessageBox::information(0,QString::number(i),QString::number(t.real));
                    }
                }else{
                    //QMessageBox::information(0,QString::number(i),"not same");
                    V.clear();
                    for(unsigned c=0; c< colID.size(); c++){
                        if(varTypes.at(colID[c])!="String"){
                            t.real = VM[i][colID[c]].real;
                            V.push_back(t);
                        }else{
                            t.string = VM[i][colID[c]].string;
                            V.push_back(t);
                        }
                    }
                    VMAgg4[0].push_back(V);

                    for(unsigned ct=1; ct< VMAgg4.size(); ct++){
                        V.clear();
                        t.real = VM[i][colTarget[ct-1]].real;
                        V.push_back(t);
                        VMAgg4[ct].push_back(V);
                        //QMessageBox::information(0,QString::number(i),QString::number(t.real));
                    }
                }
            }
        }
    }

    #pragma omp parallel sections
    {
        #pragma omp section
        {
            //1&2
            bool same12 = false;
            for(unsigned i=0; i<VMAgg2[0].size(); i++){
                for(unsigned j=0; j<VMAgg2[0][i].size(); j++){
                    //QMessageBox::information(0,"",QString::number(VMAgg1[0][VMAgg1[0].size()-1][j].real)+"-->"+QString::number(VMAgg2[0][i][j].real));
                    if(VMAgg1[0][VMAgg1[0].size()-1][j].real == VMAgg2[0][i][j].real){
                        same12 = true;
                    }else{
                        same12 = false;
                        break;
                    }
                }
                if(same12){
                    for(unsigned ct = 1; ct< VMAgg1.size(); ct++){
                        for(unsigned j=0; j<VMAgg2[ct][i].size(); j++){
                            //QMessageBox::information(0,"same", QString::number(VMAgg2[ct][i][j].real));
                            VMAgg1[ct][VMAgg1[ct].size()-1].push_back(VMAgg2[ct][i][j]);
                        }
                    }

                }else{
                    //QMessageBox::information(0,"not same","");
                    VMAgg1[0].push_back(VMAgg2[0][i]);
                    for(unsigned ct = 1; ct< VMAgg1.size(); ct++){
                        VMAgg1[ct].push_back(VMAgg2[ct][i]);
                    }
                }
            }
        }
        #pragma omp section
        {
            //3&4
            bool same34 = false;
            for(unsigned i=0; i<VMAgg4[0].size(); i++){
                for(unsigned j=0; j<VMAgg4[0][i].size(); j++){
                    //QMessageBox::information(0,"",QString::number(VMAgg3[0][VMAgg3[0].size()-1][j].real)+"-->"+QString::number(VMAgg4[0][i][j].real));
                    if(VMAgg3[0][VMAgg3[0].size()-1][j].real == VMAgg4[0][i][j].real){
                        same34 = true;
                    }else{
                        same34 = false;
                        break;
                    }
                }
                if(same34){
                    for(unsigned ct = 1; ct< VMAgg3.size(); ct++){
                        for(unsigned j=0; j<VMAgg4[ct][i].size(); j++){
                            //QMessageBox::information(0,"same", QString::number(VMAgg4[ct][i][j].real));
                            VMAgg3[ct][VMAgg3[ct].size()-1].push_back(VMAgg4[ct][i][j]);
                        }
                    }

                }else{
                    //QMessageBox::information(0,"not same","");
                    VMAgg3[0].push_back(VMAgg4[0][i]);
                    for(unsigned ct = 1; ct< VMAgg3.size(); ct++){
                        VMAgg3[ct].push_back(VMAgg4[ct][i]);
                    }
                }
            }
        }
    }

    bool same13 = false;
    for(unsigned i=0; i<VMAgg3[0].size(); i++){
        for(unsigned j=0; j<VMAgg3[0][i].size(); j++){
            //QMessageBox::information(0,"",QString::number(VMAgg1[0][VMAgg1[0].size()-1][j].real)+"-->"+QString::number(VMAgg3[0][i][j].real));
            if(VMAgg1[0][VMAgg1[0].size()-1][j].real == VMAgg3[0][i][j].real){
                same13 = true;
            }else{
                same13 = false;
                break;
            }
        }
        if(same13){
            for(unsigned ct = 1; ct< VMAgg1.size(); ct++){
                for(unsigned j=0; j<VMAgg3[ct][i].size(); j++){
                    //QMessageBox::information(0,"same", QString::number(VMAgg3[ct][i][j].real));
                    VMAgg1[ct][VMAgg1[ct].size()-1].push_back(VMAgg3[ct][i][j]);
                }
            }

        }else{
            //QMessageBox::information(0,"not same","");
            VMAgg1[0].push_back(VMAgg3[0][i]);
            for(unsigned ct = 1; ct< VMAgg1.size(); ct++){
                VMAgg1[ct].push_back(VMAgg3[ct][i]);
            }
        }
    }
    tp4a = omp_get_wtime() - tp4a;
    qDebug("Parallel 4");
    qDebug()<<tp4a;
    tp4 = omp_get_wtime()-tp4;

    VMAgg = VMAgg1;
    if(ui->serNparRB->isChecked()){
        //QMessageBox::information(0,"Compare", "Ser: "+QString::number(ts)+"\nPar 2: "+QString::number(tp2)+"\nPar 4: "+QString::number(tp4));
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
    //----------- Finish
    double tfunc = omp_get_wtime();
    std::vector<type> Vresult;
    std::vector<type> V;
    type t;
    VM.clear();
    for(unsigned c=0; c< VMAgg[0].size(); c++){
        VM.push_back(VMAgg[0][c]); //Add col ID
    }

    QStringList varNames1;
    QStringList varType1;
    for(unsigned i=0; i< colID.size(); i++){
        QString varTypeID = varTypes.at(colID[i]);
        varType1.push_back(varTypeID);
        varNames1.push_back(varNames.at(colID[i]));
    }
    varTypes = varType1;
    VM = dm->V2RowtoColType(VM, varTypes);

    for(unsigned ct=1; ct< VMAgg.size(); ct++){
        Vresult.clear();
        for(unsigned i=0; i<VMAgg[ct].size(); i++){
            double result = 0;

            if(sFunction == "SUM"){
                for(unsigned j=0; j<VMAgg[ct][i].size(); j++){
                    result = VMAgg[ct][i][j].real+result;
                }
            }else if(sFunction == "COUNT"){
                for(unsigned j=0; j<VMAgg[ct][i].size(); j++){
                    result++;
                }
            }else if(sFunction == "MEAN"){
                for(unsigned j=0; j<VMAgg[ct][i].size(); j++){
                    result = VMAgg[ct][i][j].real+result;
                }
                result = result/VMAgg[ct][i].size();
            }else if(sFunction == "MEDIAN"){
                V.clear();
                for(unsigned j=0; j<VMAgg[ct][i].size(); j++){
                    t.real = VMAgg[ct][i][j].real;
                    V.push_back(t);
                }
                V = mergesort1(V,4, true);
                if(V.size()%2 != 0){
                    result = V[(V.size()-1)/2].real;
                }else{
                    result = (V[(V.size()-1)/2].real + V[(V.size())/2].real)/2;
                }
            }else if(sFunction == "MIN"){
                V.clear();
                for(unsigned j=0; j<VMAgg[ct][i].size(); j++){
                    t.real = VMAgg[ct][i][j].real;
                    V.push_back(t);
                }
                V = mergesort1(V, 4,true);
                result = (V[0].real);

            }else if(sFunction == "MAX"){
                V.clear();
                for(unsigned j=0; j<VMAgg[ct][i].size(); j++){
                    t.real = VMAgg[ct][i][j].real;
                    V.push_back(t);
                }
                V = mergesort1(V,4, false);
                result = (V[0].real);

            }else if(sFunction == "FIRST"){
                result = (VMAgg[ct][i][0].real);
            }else if(sFunction == "LAST"){
                result = (VMAgg[ct][i][VMAgg[ct][i].size()-1].real);
            }else if(sFunction.contains("ABOVE")){
                V.clear();
                for(unsigned j=0; j<VMAgg[ct][i].size(); j++){
                    if(VMAgg[ct][i][j].real > above){
                        t.real = VMAgg[ct][i][j].real;
                        V.push_back(t);
                    }
                }
                if(sFunction =="PERCENTAGE ABOVE"){
                    result = V.size()*100/(VMAgg[ct][i].size());
                }else{
                    result = V.size()/(VMAgg[ct][i].size());
                }
            }else if(sFunction.contains("BELOW")){
                V.clear();
                for(unsigned j=0; j<VMAgg[ct][i].size(); j++){
                    if(VMAgg[ct][i][j].real < below){
                        t.real = VMAgg[ct][i][j].real;
                        V.push_back(t);
                    }
                }
                if(sFunction =="PERCENTAGE BELOW"){
                    result = V.size()*100/(VMAgg[ct][i].size());
                }else{
                    result = V.size()/(VMAgg[ct][i].size());
                }
            }else if(sFunction.contains("INSIDE")){
                V.clear();
                for(unsigned j=1; j<VMAgg[ct][i].size(); j++){
                    if(VMAgg[ct][i][j].real > inside1 && VMAgg[ct][i][j].real< inside2){
                        t.real = VMAgg[ct][i][j].real;
                        V.push_back(t);
                    }
                }
                if(sFunction =="PERCENTAGE INSIDE"){
                    result = V.size()*100/(VMAgg[ct][i].size());
                }else{
                    result = V.size()/(VMAgg[ct][i].size());
                }
            }else if(sFunction.contains("OUTSIDE")){
                V.clear();
                for(unsigned j=1; j<VMAgg[ct][i].size(); j++){
                    if(VMAgg[ct][i][j].real < outside1 && VMAgg[ct][i][j].real > outside2){
                        t.real = VMAgg[ct][i][j].real;
                        V.push_back(t);
                    }
                }
                if(sFunction =="PERCENTAGE OUTSIDE"){
                    result = V.size()*100/(VMAgg[ct][i].size());
                }else{
                    result = V.size()/(VMAgg[ct][i].size());
                }
            }else{
                //Belum dibuat atau salah
                QMessageBox::information(0, "", "SALAH");
                return;
            }
            t.real = result;
            Vresult.push_back(t);
        }
        VM.push_back(Vresult);
    }

    for(unsigned i=0; i< colTarget.size(); i++){
        varTypes.push_back("Real");
        varNames1.push_back("Agg_"+varNames.at(colTarget[i]));
    }
    varNames = varNames1;
    dataEditor->setVarNames(varNames);
    dataEditor->setVarTypes(varTypes);

    VM = dm->V2ColtoRowType(VM,varTypes);
    delete dm;

    tfunc = omp_get_wtime() - tfunc;
    qDebug("Compute function");
    qDebug() << tfunc;

    double tc2 = omp_get_wtime();
    Rcpp::DataFrame DF;
    for(unsigned j =0; j< VM[0].size();j++ ){
        if(varTypes.at(j)=="String"){
            std::vector<std::string> V;
            for(unsigned i=0; i< VM.size(); i++){
                V.push_back(VM[i][j].string);
            }
            DF.push_back(V);
        }
        else if(varTypes.at(j)=="Integer"){
            std::vector<int> Vi;
            for(unsigned i=0; i< VM.size(); i++){
                Vi.push_back(VM[i][j].real);
            }
            DF.push_back(Vi);
        }else{
            std::vector<double> Vd;
            for(unsigned i=0; i< VM.size(); i++){
                Vd.push_back(VM[i][j].real);
            }
            DF.push_back(Vd);
        }
    }
    qDebug("After add DF");
    Rcpp::StringVector colNames;
    for(int i=0; i<varNames.size(); i++){
        colNames.push_back(varNames[i].toStdString());
    }
    DF.attr("names") = colNames;
    //Rcpp::DataFrame x;
    Rcpp::Language call("as.data.frame",DF);
    dataFrame = call.eval();
    tc2 = omp_get_wtime()-tc2;
    qDebug("V to DF");
    qDebug() << tc2;

    if(ui->sameDS_RB->isChecked()){
        //Replace dataset yg sama
        m_R[ui->datasetCB->currentText().toStdString()] = dataFrame;

        //Update active dataset
        for(int i=0; i< activeDS->rowCount(); i++){
            if(!activeDS->verticalHeaderItem(i)->text().isEmpty()){
                QString dataName = ui->datasetCB->currentText();
                if(activeDS->item(i,2)->text() == dataName){
                    qDebug("Update Active DataSet");
                    int oldCol = activeDS->item(i,3)->text().split(" variable").at(0).toInt();
                    Rcpp::StringVector colNames = m_R.parseEval("colnames("+dataName.toStdString()+")");
                    QString extension, path;
                    for(int j=0; j<activeDS->getDatasetNameList().size(); j++){
                        if(dataName == activeDS->getDatasetNameList()[j]){
                            path = activeDS->getPathList()[j];
                            extension  = activeDS->getExtList()[j];
                            break;
                        }
                    }
                    int row = VM.size();
                    int col=colNames.size();
                    activeDS->updateRow(i,oldCol,colNames,dataName,dataName,extension,path,col,row);
                    break;
                }
            }
        }
        //reload tabel
        tc2 = omp_get_wtime();
        activeDS->getVarEditor()->setDataEditor(dataEditor);
        activeDS->getVarEditor()->setDatasetName(ui->datasetCB->currentText());
        activeDS->getVarEditor()->reloadVar(varNames, varTypes);
        dataEditor->setVM(VM);
        dataEditor->reloadTable(true,false);
        //activeDS->reloadData(ui->datasetCB->currentText());
        tc2 = omp_get_wtime()-tc2;
        qDebug("Reload tabel");
        qDebug() << tc2;


    }else{
        //Buat dataset baru
        QString name;
        if(ui->newDatasetName->text().isEmpty()){
            name = "Aggregate_"+ui->datasetCB->currentText();
        }else{
            name = ui->newDatasetName->text();
        }
        m_R[name.toStdString()] = dataFrame; //set ke active dataset class

        //add new dataset
        int row = VM.size();
        int col = VM[0].size();
        QString ext, path;
        for(int i=0; i< activeDS->getExtList().size();i++){
            if(ui->datasetCB->currentText() == activeDS->getDatasetNameList().at(i)){
                ext = activeDS->getExtList().at(i);
                path = activeDS->getPathList().at(i);
            }
        }
        activeDS->addRowData(colNames,name,ext,path,col,row);
        //reload tabel
        tc2 = omp_get_wtime();
        //activeDS->reloadData(name);
        activeDS->getVarEditor()->setDataEditor(dataEditor);
        activeDS->getVarEditor()->setDatasetName(name);
        activeDS->getVarEditor()->reloadVar(varNames, varTypes);
        dataEditor->setVM(VM);
        dataEditor->reloadTable(true, false);
        tc2 = omp_get_wtime()-tc2;
        qDebug("Reload tabel");
        qDebug() << tc2;
        qDebug("Diff Dataset");
        //MainWindow *mw = (MainWindow*) QApplication::activeWindow();
        //mw->setCurrentFile(name+"."+ext);
    }
    */

}

std::vector<std::vector<std::vector<type> > > DMAggregateDialog::aggregate1(std::vector<std::vector<type> > VM, int low, int high, std::vector<int> colID, std::vector<int> colTarget)
{
    std::vector<std::vector<std::vector<type> > > VMAgg;
    std::vector<std::vector<type> > VColID, VColTarget;
    std::vector<type> V;
    type t;
    bool same = false;
    for(unsigned c=0; c< colID.size(); c++){
        if(varTypes.at(colID[c])!="String"){
            t.real = VM[low][colID[c]].real;
            V.push_back(t);
        }else{
            t.string = VM[low][colID[c]].string;
            V.push_back(t);
        }
    }
    VColID.push_back(V);
    VMAgg.push_back(VColID);

    for(unsigned ct=0; ct< colTarget.size(); ct++){
        V.clear();
        VColTarget.clear();

        t.real = VM[low][colTarget[ct]].real;
        V.push_back(t);
        VColTarget.push_back(V);
        VMAgg.push_back(VColTarget);
    }

    for(int i=low+1; i<high; i++){
        for(unsigned c=0; c< colID.size(); c++){
            if(varTypes.at(colID[c])!="String"){
                if(VM[i][colID[c]].real != VM[i-1][colID[c]].real){
                    same = false;
                    break;
                }else{
                    same = true;
                }
            }else{
                if(VM[i][colID[c]].string != VM[i-1][colID[c]].string){
                    same = false;
                    break;
                }else{
                    same = true;
                }
            }
        }
        if(same){
            //QMessageBox::information(0,QString::number(i),"same");
            for(unsigned ct=1; ct < VMAgg.size(); ct++){
                t.real = VM[i][colTarget[ct-1]].real;
                VMAgg[ct][VMAgg[ct].size()-1].push_back(t);
                //QMessageBox::information(0,QString::number(i),QString::number(t.real));
            }
        }else{
            //QMessageBox::information(0,QString::number(i),"not same");
            V.clear();
            for(unsigned c=0; c< colID.size(); c++){
                if(varTypes.at(colID[c])!="String"){
                    t.real = VM[i][colID[c]].real;
                    V.push_back(t);
                }else{
                    t.string = VM[i][colID[c]].string;
                    V.push_back(t);
                }
            }
            VMAgg[0].push_back(V);

            for(unsigned ct=1; ct< VMAgg.size(); ct++){
                V.clear();
                t.real = VM[i][colTarget[ct-1]].real;
                V.push_back(t);
                VMAgg[ct].push_back(V);
                //QMessageBox::information(0,QString::number(i),QString::number(t.real));
            }
        }

    }
    return VMAgg;
}

std::vector<type> DMAggregateDialog::mergesort1(std::vector<type> &vec, int threads, bool order)
{
    return vec;
    /*if(vec.size() <= 1)
    {
        return vec;
    }

    // Determine the location of the middle element in the vector
    std::vector<type> ::iterator middle = vec.begin() + (vec.size() / 2);

    if(threads < 4){
        std::vector<type> left(vec.begin(), middle);
        std::vector<type> right(middle, vec.end());
        if(threads ==1){
            left = mergesort1(left, 1,order);
            right = mergesort1(right, 1, order);
        }else{
            #pragma omp parallel sections
            {
              #pragma omp section
              {
                    left = mergesort1(left, threads/2,order);
              }
              #pragma omp section
              {
                    right = mergesort1(right, threads - threads/2, order);
              }
            }
        }
        return merge1(left, right,order);
    }else{
        std::vector<type> ::iterator middle1 = vec.begin() + (vec.size() / 4);
        std::vector<type> ::iterator middle2 = middle + (vec.size() / 4);

        std::vector<type> left1(vec.begin(), middle1);
        std::vector<type> left2(middle1, middle);
        std::vector<type> right1(middle, middle2);
        std::vector<type> right2(middle2, vec.end());

        #pragma omp parallel sections
        {
          #pragma omp section
          {
                left1 = mergesort1(left1, threads/4,order);
          }
          #pragma omp section
          {
                left2 = mergesort1(left2, threads/4, order);
          }
          #pragma omp section
          {
                right1 = mergesort1(right1, threads/4, order);
          }
          #pragma omp section
          {
                right2 = mergesort1(right2, threads/4, order);
          }
        }
        std::vector<type> V1, V2;
        #pragma omp parallel sections
        {
          #pragma omp section
          {
                V1 = merge1(left1, left2, order);
          }
          #pragma omp section
          {
                V2 =  merge1(right1, right2, order);
          }
        }
        return merge1(V1,V2, order);
    }*/
}

std::vector<type> DMAggregateDialog::merge1(const std::vector<type> &left, const std::vector<type> &right, bool order)
{
    std::vector<type> result;
    unsigned left_it = 0, right_it = 0;

    while(left_it < left.size() && right_it < right.size())
    {
        if(left[left_it].real < right[right_it].real)
        {
            if(order){
                result.push_back(left[left_it]);
                left_it++;
            }
            else{
                result.push_back(right[right_it]);
                right_it++;
            }
        }
        else if(left[left_it].real > right[right_it].real)
        {
            if(order){
                result.push_back(right[right_it]);
                right_it++;
            }
            else{
                result.push_back(left[left_it]);
                left_it++;
            }
        }else{
            result.push_back(left[left_it]);
            left_it++;
        }
    }
    // Push the remaining data from both vectors onto the resultant
    while(left_it < left.size())
    {
        result.push_back(left[left_it]);
        left_it++;
    }
    while(right_it < right.size())
    {
        result.push_back(right[right_it]);
        right_it++;
    }
    return result;
}
