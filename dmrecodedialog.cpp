#include "dmrecodedialog.h"
#include "ui_dmrecodedialog.h"
#include "mainwindow.h"

DMRecodeDialog::DMRecodeDialog(QWidget *parent, RInside &R) :
    QDialog(parent),
    ui(new Ui::DMRecodeDialog),
    m_R(R)
{
    ui->setupUi(this);
    connect(ui->selectButton, SIGNAL(clicked()), this, SLOT(selectVar()));
    connect(ui->pointRB,SIGNAL(clicked()),this,SLOT(selectTypeVal()));
    connect(ui->intvRB,SIGNAL(clicked()),this,SLOT(selectTypeVal()));
    connect(ui->addValButton,SIGNAL(clicked()),this,SLOT(addVal()));
    connect(ui->removeButton,SIGNAL(clicked()),this,SLOT(removeVal()));
    connect(ui->elseValCB,SIGNAL(clicked()),this,SLOT(elseVal()));
    connect(ui->renameButton,SIGNAL(clicked()),this,SLOT(renameVar()));
    connect(this,SIGNAL(accepted()),this,SLOT(recodeSameVar()));

    ui->serNparRB->setChecked(true);
    ui->pointRB->setChecked(true);
    selectTypeVal();
    ui->groupBox->setEnabled(false);
    ui->valListWidget->setEnabled(false);
    ui->elseVal->setEnabled(false);
    ui->elseValCB->setEnabled(false);

    ui->renameButton->hide();
}

DMRecodeDialog::~DMRecodeDialog()
{
    delete ui;
}

void DMRecodeDialog::setDataDisplayedName(QString name)
{
    dataDisplayedName = name;
}

void DMRecodeDialog::setDE(DataEditor *de)
{
    dataEditor = de;
}

void DMRecodeDialog::setActiveDS(ActiveDataset *ads)
{
    activeDS = ads;
}

void DMRecodeDialog::setCBDatasetName(QStringList list)
{
    ui->datasetCB->clear();
    ui->datasetCB->addItems(list);
}

void DMRecodeDialog::setCurrentCBDSName(int index)
{
    ui->datasetCB->setCurrentIndex(index);
    selectDataset(index);
    connect(ui->datasetCB,SIGNAL(currentIndexChanged(int)),this,SLOT(selectDataset(int)));
}

void DMRecodeDialog::selectDataset(int index)
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
    //double t=omp_get_wtime();
    //setVarTypesFromDF(ui->datasetCB->itemText(index).toStdString());
    //qDebug("VT");
    //qDebug() << omp_get_wtime()-t;
}

void DMRecodeDialog::selectVar()
{
    if(ui->listLeftVarWidget->currentRow()!=-1){
        moveCurrentItem(ui->listLeftVarWidget, ui->listRightVarWidget);
    }else{
        moveCurrentItem(ui->listRightVarWidget, ui->listLeftVarWidget);
    }

    if(ui->listRightVarWidget->count()>0){
        ui->groupBox->setEnabled(true);
        ui->valListWidget->setEnabled(true);
        ui->elseValCB->setEnabled(true);
    }else{
        ui->groupBox->setEnabled(false);
        ui->valListWidget->setEnabled(false);
        ui->elseVal->setEnabled(false);
        ui->elseValCB->setEnabled(false);
    }
}

void DMRecodeDialog::moveCurrentItem(QListWidget *source, QListWidget *target)
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

void DMRecodeDialog::selectTypeVal()
{
    if(ui->pointRB->isChecked()){
        ui->oldValPoint->setEnabled(true);

        ui->oldValItv1->setEnabled(false);
        ui->oldValItv2->setEnabled(false);
    }else{
        ui->oldValPoint->setEnabled(false);

        ui->oldValItv1->setEnabled(true);
        ui->oldValItv2->setEnabled(true);
    }
}

void DMRecodeDialog::addVal()
{
    newVal.push_back(ui->newVal->text());
    ui->newVal->clear();
    if(ui->pointRB->isChecked()){
        oldVal.push_back(ui->oldValPoint->text());
        ui->oldValPoint->clear();

        ui->valListWidget->addItem(oldVal[oldVal.size()-1]+" -> "+newVal[newVal.size()-1]);
        ui->intvRB->setCheckable(false);
    }else{
        oldValLow.push_back(ui->oldValItv1->text());
        oldValHigh.push_back(ui->oldValItv2->text());
        ui->oldValItv1->clear();
        ui->oldValItv2->clear();

        ui->valListWidget->addItem(oldValLow[oldValLow.size()-1]+" to "+oldValHigh[oldValHigh.size()-1]+" -> "+newVal[newVal.size()-1]);
        ui->pointRB->setCheckable(false);
    }
}

void DMRecodeDialog::removeVal()
{
    QList<QListWidgetItem*> l = ui->valListWidget->selectedItems();
    ui->valListWidget->setCurrentRow(-1);
    if (! l.empty()) {
        for(int i=0;i<l.length();i++){
            delete l.at(i);
        }
        l.clear();
    }
    if(ui->valListWidget->count()==0){
        ui->intvRB->setCheckable(true);
        ui->pointRB->setCheckable(true);
    }
}

void DMRecodeDialog::elseVal()
{
    if(ui->elseValCB->isChecked()){
        ui->elseVal->setEnabled(true);
    }else{
        ui->elseVal->setEnabled(false);
    }
}

void DMRecodeDialog::renameVar()
{
    QString newName = QInputDialog::getText(this,"Rename Selected Variable","New Var Name");
    if(!newName.isEmpty()){
        ui->listRightVarWidget->selectedItems().at(0)->setText(newName);
        for(int i=0; i<varNames.size(); i++){
            if(varNames[i] == ui->listRightVarWidget->selectedItems().at(0)->text()){
                varNames.replace(i,newName);
            }
        }
    }
}

QString DMRecodeDialog::checkVarType(QStringList V) //Masih ngecek indeks pertama aja
{
    QRegExp re("[+-]?\\d*\\.?\\d+");
    for(int i=0; i<V.size(); i++){
        QString string = V[0];
        if(!re.exactMatch(string)){
            //string
            return "String";
        }else{
            //numeric
            bool validate;
            string.toInt(&validate);
            if(validate == false && !re.exactMatch(string)){
                return "String";
            }
            return "Integer";

            string.toDouble(&validate);
            if(validate == false && !re.exactMatch(string)){
                return "String";
            }
            return "Real";
        }
    }
    return "Unknown";
}

void DMRecodeDialog::recodeSameVar()
{
    qDebug("------- Recode SameVar "+ ui->datasetCB->currentText().toLocal8Bit() +"---------");
    //String masih case sensitive
    /*QProgressDialog progress("Recoding...","Cancel" , 0, 100, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setWindowFlags(Qt::SplashScreen);
    QList<QPushButton *> L=progress.findChildren<QPushButton *>();
    L.at(0)->hide();
    progress.setValue(0);*/
    /*
    DataManagement *dm = new DataManagement;
    if(ui->listRightVarWidget->count()<1){
        return;
    }
    std::vector<int> cols;
    std::vector<bool> order;
    for(int i=0; i< varNames.size(); i++){
        for(int j=0; j< ui->listRightVarWidget->count(); j++){
            if(varNames[i] == ui->listRightVarWidget->item(j)->text()){
                cols.push_back(i);
                order.push_back(true);
            }
        }
    }
    if(ui->elseValCB->isChecked()){
        newVal.push_back(ui->elseVal->text());
        ui->elseVal->clear();
    }

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

    double time1, time1a, time2, time2a;
    if(ui->serNparRB->isChecked()){
        time1a = omp_get_wtime();
        std::vector<std::vector<type> > VM1;
        for(unsigned i = 0; i<VM.size(); i++){
            type t;
            t.real = i+1;
            VM[i].push_back(t); //Add row number
            VM1.push_back(VM[i]);
            VM[i].pop_back(); //Remove row number
        }

        time1 = omp_get_wtime();
        for(unsigned i=0; i<cols.size(); i++){
            std::vector<int> colSingle;
            colSingle.push_back(cols[i]);
            double s = omp_get_wtime();
            //VM1 = mergesort(VM1,1,colSingle, order);
            QStringList types;
            types.push_back(varTypes[cols[i]]);
            dm->mergeSort(VM1,colSingle,order,types,1);
            s = omp_get_wtime() -s;
            qDebug("Sort Serial:");
            qDebug() << s;
            double ts1 = omp_get_wtime();
            if(ui->intvRB->isChecked()){
                bsr_interval(VM1, VM,0,VM1.size()-1,cols[i],oldValLow,oldValHigh, newVal);
            }else{
                bsearch_recode(VM1, VM,0,VM1.size()-1,cols[i],oldVal, newVal);
            }
            ts1 = omp_get_wtime() - ts1;
            qDebug("Serial");
            qDebug()<<ts1;
        }
        time1 = omp_get_wtime()-time1;
        time1a = omp_get_wtime()-time1a;
        qDebug("-Serial done-");
        qDebug() << time1a;
        //-------------------------
        //VM.clear();
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
        qDebug("Before 2 core");
        //---------------------------------------------------------2Core
        std::vector<std::vector<type> > MP1,MP2;
        time2a = omp_get_wtime();
        for(unsigned i = 0; i<VM.size(); i++){
            type t;
            t.real = i+1;
            VM[i].push_back(t); //Add row number
            if(i<VM.size()/2){
                MP1.push_back(VM[i]);
            }else if(i<VM.size()){
                MP2.push_back(VM[i]);
            }
            VM[i].pop_back(); //Remove row number
        }
        qDebug("--1");
        time2 = omp_get_wtime();

        for(unsigned i=0; i<cols.size(); i++){
            std::vector<int> colSingle;
            colSingle.push_back(cols[i]);
            double s=omp_get_wtime();
            //MP1 = mergesort(MP1,2,colSingle, order);
            //MP2 = mergesort(MP2,2,colSingle, order);
            qDebug("--2");
            QStringList types;
            qDebug("--3");
            types.push_back(varTypes[cols[i]]);
            qDebug("--4");
            qDebug() << types.size();
            qDebug() << colSingle.size();

            dm->mergeSort(MP1,colSingle,order,types,2);
            qDebug("--5");
            dm->mergeSort(MP2,colSingle,order,types,2);

            s = omp_get_wtime() -s;
            qDebug("Sort Par2:");
            qDebug() << s;
            double tp1 = omp_get_wtime();
            #pragma omp parallel sections //shared(VM)
            {
                #pragma omp section
                {
                    if(ui->intvRB->isChecked()){
                        bsr_interval(MP1, VM,0,MP1.size()-1,cols[i],oldValLow,oldValHigh, newVal);
                    }else{
                        bsearch_recode(MP1, VM,0,MP1.size()-1,cols[i],oldVal, newVal);
                    }
                }
                #pragma omp section
                {
                    if(ui->intvRB->isChecked()){
                        bsr_interval(MP2, VM,0,MP2.size()-1,cols[i],oldValLow,oldValHigh, newVal);
                    }else{
                        bsearch_recode(MP2, VM,0,MP2.size()-1,cols[i],oldVal, newVal);
                    }
                }
            }
            tp1 = omp_get_wtime() - tp1;
            qDebug("Parallel 2Core");
            qDebug()<<tp1;

        }
        time2 = omp_get_wtime() - time2;
        time2a = omp_get_wtime() - time2a;
        qDebug("-Parallel 2 done-");
        qDebug() << time2a;
        //-------------------------

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
        MP1.clear();
        MP2.clear();
    }
    //-----------------------------------------------4Core
    std::vector<std::vector<type> > MP1,MP2, MP3, MP4;
    double time4a = omp_get_wtime();
    for(unsigned i = 0; i<VM.size(); i++){
        type t;
        t.real = i+1;
        VM[i].push_back(t); //Add row number
        if(i<VM.size()/4){
            MP1.push_back(VM[i]);
        }else if(i<VM.size()/2){
            MP2.push_back(VM[i]);
        }else if(i<VM.size()*3/4){
            MP3.push_back(VM[i]);
        }else{
            MP4.push_back(VM[i]);
        }
        VM[i].pop_back(); //Remove row number
    }

    double time4 = omp_get_wtime();

    for(unsigned i=0; i<cols.size(); i++){
        std::vector<int> colSingle;
        colSingle.push_back(cols[i]);
        double s = omp_get_wtime();
        //MP1 = mergesort(MP1,4,colSingle, order);
        //MP2 = mergesort(MP2,4,colSingle, order);
        //MP3 = mergesort(MP3,4,colSingle, order);
        //MP4 = mergesort(MP4,4,colSingle, order);
        QStringList types;
        types.push_back(varTypes[cols[i]]);
        dm->mergeSort(MP1,colSingle,order,types,4);
        dm->mergeSort(MP2,colSingle,order,types,4);
        dm->mergeSort(MP3,colSingle,order,types,4);
        dm->mergeSort(MP4,colSingle,order,types,4);

        s = omp_get_wtime() -s;
        qDebug("Sort Par 4:");
        qDebug() << s;
        double tp4 = omp_get_wtime();
        #pragma omp parallel sections //shared(VM)
        {
            #pragma omp section
            {
                if(ui->intvRB->isChecked()){
                    bsr_interval(MP1, VM,0,MP1.size()-1,cols[i],oldValLow,oldValHigh, newVal);
                }else{
                    bsearch_recode(MP1, VM,0,MP1.size()-1,cols[i],oldVal, newVal);
                }

            }
            #pragma omp section
            {
                if(ui->intvRB->isChecked()){
                    bsr_interval(MP2, VM,0,MP2.size()-1,cols[i],oldValLow,oldValHigh, newVal);
                }else{
                    bsearch_recode(MP2, VM,0,MP2.size()-1,cols[i],oldVal, newVal);
                }
            }
            #pragma omp section
            {
                if(ui->intvRB->isChecked()){
                    bsr_interval(MP3, VM,0,MP3.size()-1,cols[i],oldValLow,oldValHigh, newVal);
                }else{
                    bsearch_recode(MP3, VM,0,MP3.size()-1,cols[i],oldVal, newVal);
                }
            }
            #pragma omp section
            {
                if(ui->intvRB->isChecked()){
                    bsr_interval(MP4, VM,0,MP4.size()-1,cols[i],oldValLow,oldValHigh, newVal);
                }else{
                    bsearch_recode(MP4, VM,0,MP4.size()-1,cols[i],oldVal, newVal);
                }
            }
        }
        tp4 = omp_get_wtime() - tp4;
        qDebug("Parallel 4 Core");
        qDebug()<<tp4;

    }
    time4 = omp_get_wtime() - time4;
    time4a = omp_get_wtime() - time4a;
    qDebug("-Parallel 4 done-");
    qDebug() << time4a;

    qDebug("=====FINISH====");
    if(ui->serNparRB->isChecked()){
        qDebug("Speedup");
        qDebug("Par2");
        qDebug() << time1a/(time2a);
        qDebug("Par4");
        qDebug() << time1a/(time4a);
        qDebug("Eff");
        qDebug("Par2");
        qDebug() << time1a/(2*time2a);
        qDebug("Par4");
        qDebug() << time1a/(4*time4a);
    }

    //if(ui->loadDECheckBox->isChecked()){
    if(dataEditor->isHidden() && ui->datasetCB->currentText() != dataDisplayedName){
        //setVtoDF:
        //Vector to DataFrame
        double tc2 = omp_get_wtime();
        for(unsigned j =0; j< VM[0].size();j++ ){
            std::vector<std::string> V;
            std::vector<int> Vi;
            std::vector<double> Vd;
            if(varTypes[j]=="String"){
                for(unsigned i=0; i< VM.size(); i++){
                    V.push_back(VM[i][j].string);
                }
                dataFrame[j] = V;
            }else if(varTypes.at(j)=="Integer"){
                for(unsigned i=0; i< VM.size(); i++){
                    Vi.push_back(VM[i][j].real);
                }
                dataFrame[j] = Vi;
            }else{
                for(unsigned i=0; i< VM.size(); i++){
                    Vd.push_back(VM[i][j].real);
                }
                dataFrame[j] = Vd;
            }
        }
        m_R[ui->datasetCB->currentText().toStdString()] = dataFrame; //setkeR
        tc2 = omp_get_wtime()-tc2;
        qDebug("V to DF");
        qDebug() << tc2;
        //if(ui->loadDECheckBox->isChecked()){
            activeDS->reloadData(ui->datasetCB->currentText());
        //}else{
            //goto finishNotLoad;
        //}
    }else{
        double time0a = omp_get_wtime();
        if(dataDisplayedName == ui->datasetCB->currentText()){
            activeDS->getVarEditor()->setDataEditor(dataEditor);
            activeDS->getVarEditor()->reloadVar(varNames,varTypes);
            dataEditor->setVM(VM);
            for(unsigned j = 0; j< cols.size(); j++){
                std::vector<type> V;
                type t;
                if(varTypes[cols[j]] == "String"){
                    qDebug("String");
                    for(unsigned i=0; i<VM.size(); i++){
                        t.string = VM[i][cols[j]].string;
                        V.push_back(t);
                    }
                    dataEditor->loadVarStr(V,cols[j],varNames[cols[j]],varTypes[cols[j]]);
                }else{
                    qDebug("Numeric");
                    for(unsigned i=0; i<VM.size(); i++){
                        t.real = VM[i][cols[j]].real;
                        V.push_back(t);
                    }
                    dataEditor->loadVarReal(V,cols[j],varNames[cols[j]], varTypes[cols[j]]);
                }
            }
            //dataEditor->clearIdxChanged();
        }else{
            //activeDS->reloadData(ui->datasetCB->currentText());
            activeDS->getVarEditor()->setDataEditor(dataEditor);
            activeDS->getVarEditor()->setDatasetName(ui->datasetCB->currentText());
            activeDS->getVarEditor()->reloadVar(varNames,varTypes);
            dataEditor->setVM(VM);
            dataEditor->reloadTable(false,false);
            //kalau ada rename nama var --> update ke Active Dataset & dataeditor reloadTable(true)

            qDebug("Diff Dataset");
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
        time0a = omp_get_wtime() - time0a;
        qDebug("Reload");
        qDebug() << time0a;
    }
    delete dm;
    */
}

void DMRecodeDialog::bsearch_recode(std::vector<std::vector<type> > &VM, std::vector<std::vector<type> > &VMreal, int low, int high, int col, QStringList oldVal, QStringList newVal)
{
    std::vector<type> index;
    type t;
    int low0=low, high0=high;
    if(varTypes.at(col)=="String"){
        for(int i=0; i< oldVal.size(); i++){
            low = low0;
            high = high0;
            while(low <= high){
                int mid = (low + high)/2;
                std::string key = oldVal[i].toStdString();

                if(key < VM[mid][col].string){
                    high = mid-1;
                }
                else if(key > VM[mid][col].string){
                    low = mid+1;
                }
                else{
                    QString newValType = checkVarType(newVal);
                    if(newValType == "String"){
                        VMreal[QString::number(VM[mid][VM[0].size()-1].real).toInt()-1][col].string = newVal[i].toStdString();
                        t.real = mid;
                        index.push_back(t);
                        int j = 1;
                        //cek index sebelumnya
                        while(mid-j>=low){
                            if(key == VM[mid-j][col].string){
                                VMreal[QString::number(VM[mid-j][VM[0].size()-1].real).toInt()-1][col].string = newVal[i].toStdString();
                                t.real = mid-j;
                                index.push_back(t);
                            }
                            else{
                                break;
                            }
                            j++;
                        }
                        j=1;
                        //cek index setelahnya
                        while(mid+j<=high){
                            if(key == VM[mid+j][col].string){
                                VMreal[QString::number(VM[mid+j][VM[0].size()-1].real).toInt()-1][col].string = newVal[i].toStdString();
                                t.real = mid+j;
                                index.push_back(t);
                            }
                            else{
                                break;
                            }
                            j++;
                        }
                    }else{
                        varTypes.replace(col, "Real");
                        VMreal[QString::number(VM[mid][VM[0].size()-1].real).toInt()-1][col].real = newVal[i].toDouble();
                        t.real = mid;
                        index.push_back(t);
                        int j = 1;
                        //cek index sebelumnya
                        while(mid-j>=low){
                            if(key == VM[mid-j][col].string){
                                VMreal[QString::number(VM[mid-j][VM[0].size()-1].real).toInt()-1][col].real = newVal[i].toDouble();
                                t.real = mid-j;
                                index.push_back(t);
                            }
                            else{
                                break;
                            }
                            j++;
                        }
                        j=1;
                        //cek index setelahnya
                        while(mid+j<=high){
                            if(key == VM[mid+j][col].string){
                                VMreal[QString::number(VM[mid+j][VM[0].size()-1].real).toInt()-1][col].real = newVal[i].toDouble();
                                t.real = mid+j;
                                index.push_back(t);
                            }
                            else{
                                break;
                            }
                            j++;
                        }
                    }

                    break;
                }
            }
        }
    }else{
        for(int i=0; i< oldVal.size(); i++){
            double key = oldVal[i].toDouble();
            low = low0;
            high = high0;
            while(low <= high){
                int mid = (low + high)/2;
                if(key < VM[mid][col].real){
                    high = mid-1;
                }
                else if(key > VM[mid][col].real){
                    low = mid+1;
                }
                else{
                    QString newValType = checkVarType(newVal);
                    if(newValType != "String"){
                        VMreal[QString::number(VM[mid][VM[0].size()-1].real).toInt()-1][col].real = newVal[i].toDouble();
                        t.real = mid;
                        index.push_back(t);
                        int j = 1;
                        //cek index sebelumnya
                        while(mid-j>=low){
                            if(key == VM[mid-j][col].real){
                                VMreal[QString::number(VM[mid-j][VM[0].size()-1].real).toInt()-1][col].real = newVal[i].toDouble();
                                t.real = mid-j;
                                index.push_back(t);
                            }
                            else{
                                break;
                            }
                            j++;
                        }
                        j=1;
                        //cek index setelahnya
                        while(mid+j<=high){
                            if(key == VM[mid+j][col].real){
                                VMreal[QString::number(VM[mid+j][VM[0].size()-1].real).toInt()-1][col].real = newVal[i].toDouble();
                                t.real = mid+j;
                                index.push_back(t);
                            }
                            else{
                                break;
                            }
                            j++;
                        }
                    }else{
                        varTypes.replace(col,"String");
                        VMreal[QString::number(VM[mid][VM[0].size()-1].real).toInt()-1][col].string = newVal[i].toStdString();
                        t.real = mid;
                        index.push_back(t);
                        int j = 1;
                        //cek index sebelumnya
                        while(mid-j>=low){
                            if(key == VM[mid-j][col].real){
                                VMreal[QString::number(VM[mid-j][VM[0].size()-1].real).toInt()-1][col].string = newVal[i].toStdString();
                                t.real = mid-j;
                                index.push_back(t);
                            }
                            else{
                                break;
                            }
                            j++;
                        }
                        j=1;
                        //cek index setelahnya
                        while(mid+j<=high){
                            if(key == VM[mid+j][col].real){
                                VMreal[QString::number(VM[mid+j][VM[0].size()-1].real).toInt()-1][col].string = newVal[i].toStdString();
                                t.real = mid+j;
                                index.push_back(t);
                            }
                            else{
                                break;
                            }
                            j++;
                        }
                    }
                    break;
                }
            }
        }
    }
    //Recode elseValue
    index = mergesort1(index,4,true); //--> ini yang buat agak lebih lambat parallelnya, karena baca cuma 1 core

    if(oldVal.size() < newVal.size()){
        std::string elseVal = newVal[newVal.size()-1].toStdString();
        unsigned c =0;
        for(unsigned i=0; i< VM.size(); i++){
            QString newValType = checkVarType(newVal);
            if(newValType != "String"){
                if(c<index.size()){
                    if((int) i != index[c].real){
                        VMreal[QString::number(VM[i][VM[0].size()-1].real).toInt()-1][col].real = QString::fromStdString(elseVal).toDouble();
                    }else{
                        c++;
                    }
                }else{
                    VMreal[QString::number(VM[i][VM[0].size()-1].real).toInt()-1][col].real = QString::fromStdString(elseVal).toDouble();
                }
            }
            else{
                if(c<index.size()){
                    if((int) i != index[c].real){
                        VMreal[QString::number(VM[i][VM[0].size()-1].real).toInt()-1][col].string = elseVal;
                    }else{
                        c++;
                    }
                }else{
                    VMreal[QString::number(VM[i][VM[0].size()-1].real).toInt()-1][col].string = elseVal;
                }
            }
        }
    }
}

void DMRecodeDialog::bsr_interval(std::vector<std::vector<type> > &VM, std::vector<std::vector<type> > &VMreal, int low, int high, int col, QStringList oldVal1, QStringList oldVal2, QStringList newVal)
{
    std::vector<type> index;
    type t;
    int low0=low, high0=high;
    if(varTypes.at(col)=="String"){
        for(int i=0; i< oldVal1.size(); i++){
            low = low0;
            high = high0;
            while(low <= high){
                int mid = (low + high)/2;
                std::string key1 = oldVal1[i].toStdString();
                std::string key2 = oldVal2[i].toStdString();//Key2 harus > key1

                if(key2 < VM[mid][col].string){
                    high = mid-1;
                }
                else if(key1 > VM[mid][col].string){
                    low = mid+1;
                }
                else{
                    QString newValType = checkVarType(newVal);
                    if(newValType == "String"){
                        VMreal[QString::number(VM[mid][VM[0].size()-1].real).toInt()-1][col].string = newVal[i].toStdString();
                        t.real = mid;
                        index.push_back(t);
                        int j = 1;
                        //cek index sebelumnya
                        while(mid-j>=low){
                            if(key1 <= VM[mid-j][col].string && key2 >= VM[mid-j][col].string){
                                VMreal[QString::number(VM[mid-j][VM[0].size()-1].real).toInt()-1][col].string = newVal[i].toStdString();
                                t.real = mid-j;
                                index.push_back(t);
                            }
                            else{
                                break;
                            }
                            j++;
                        }
                        j=1;
                        //cek index setelahnya
                        while(mid+j<=high){
                            if(key1 <= VM[mid+j][col].string && key2 >= VM[mid+j][col].string){
                                VMreal[QString::number(VM[mid+j][VM[0].size()-1].real).toInt()-1][col].string = newVal[i].toStdString();
                                t.real = mid+j;
                                index.push_back(t);
                            }
                            else{
                                break;
                            }
                            j++;
                        }
                    }else{
                        varTypes.replace(col,"Real");
                        VMreal[QString::number(VM[mid][VM[0].size()-1].real).toInt()-1][col].real = newVal[i].toDouble();
                        t.real = mid;
                        index.push_back(t);
                        int j = 1;
                        //cek index sebelumnya
                        while(mid-j>=low){
                            if(key1 <= VM[mid-j][col].string && key2 >= VM[mid-j][col].string){
                                VMreal[QString::number(VM[mid-j][VM[0].size()-1].real).toInt()-1][col].real = newVal[i].toDouble();
                                t.real = mid-j;
                                index.push_back(t);
                            }
                            else{
                                break;
                            }
                            j++;
                        }
                        j=1;
                        //cek index setelahnya
                        while(mid+j<=high){
                            if(key1 <= VM[mid+j][col].string && key2 >= VM[mid+j][col].string){
                                VMreal[QString::number(VM[mid+j][VM[0].size()-1].real).toInt()-1][col].real = newVal[i].toDouble();
                                t.real = mid+j;
                                index.push_back(t);
                            }
                            else{
                                break;
                            }
                            j++;
                        }
                    }

                    break;
                }
            }
        }
    }else{
        for(int i=0; i< oldVal1.size(); i++){
            low = low0;
            high = high0;
            double key1 = oldVal1[i].toDouble();
            double key2 = oldVal2[i].toDouble();

            while(low <= high){
                int mid = (low + high)/2;

                if(key2 < VM[mid][col].real){
                    high = mid-1;
                }
                else if(key1 > VM[mid][col].real){
                    low = mid+1;
                }
                else{
                    QString newValType = checkVarType(newVal);
                    if(newValType != "String"){
                        VMreal[QString::number(VM[mid][VM[0].size()-1].real).toInt()-1][col].real = newVal[i].toDouble();
                        t.real = mid;
                        index.push_back(t);
                        int j = 1;
                        //cek index sebelumnya
                        while(mid-j>=low){
                            if(key1 <= VM[mid-j][col].real && key2 >= VM[mid-j][col].real){
                                VMreal[QString::number(VM[mid-j][VM[0].size()-1].real).toInt()-1][col].real = newVal[i].toDouble();
                                t.real = mid-j;
                                index.push_back(t);
                            }
                            else{
                                break;
                            }
                            j++;
                        }
                        j=1;
                        //cek index setelahnya
                        while(mid+j<=high){
                            if(key1 <= VM[mid+j][col].real && key2 >= VM[mid+j][col].real){
                                VMreal[QString::number(VM[mid+j][VM[0].size()-1].real).toInt()-1][col].real = newVal[i].toDouble();
                                t.real = mid+j;
                                index.push_back(t);
                            }
                            else{
                                break;
                            }
                            j++;
                        }
                    }else{
                        varTypes.replace(col,"String");
                        VMreal[QString::number(VM[mid][VM[0].size()-1].real).toInt()-1][col].string = newVal[i].toStdString();
                        t.real = mid;
                        index.push_back(t);
                        int j = 1;
                        //cek index sebelumnya
                        while(mid-j>=low){
                            if(key1 <= VM[mid-j][col].real && key2 >= VM[mid-j][col].real){
                                VMreal[QString::number(VM[mid-j][VM[0].size()-1].real).toInt()-1][col].string = newVal[i].toStdString();
                                t.real = mid-j;
                                index.push_back(t);
                            }
                            else{
                                break;
                            }
                            j++;
                        }
                        j=1;
                        //cek index setelahnya
                        while(mid+j<=high){
                            if(key1 <= VM[mid+j][col].real && key2 >= VM[mid+j][col].real){
                                VMreal[QString::number(VM[mid+j][VM[0].size()-1].real).toInt()-1][col].string = newVal[i].toStdString();
                                t.real = mid+j;
                                index.push_back(t);
                            }
                            else{
                                break;
                            }
                            j++;
                        }
                    }

                    break;
                }
            }
        }
    }
    //Recode elseValue
    index = mergesort1(index,4,true);

    if(oldVal1.size() < newVal.size()){
        std::string elseVal = newVal[newVal.size()-1].toStdString();
        unsigned c =0;
        for(unsigned i=0; i< VM.size(); i++){
            QString newValType = checkVarType(newVal);
            if(newValType != "String"){
                if(c<index.size()){
                    if((int) i != index[c].real){
                        VMreal[QString::number(VM[i][VM[0].size()-1].real).toInt()-1][col].real = QString::fromStdString(elseVal).toDouble();
                    }else{
                        c++;
                    }
                }else{
                    VMreal[QString::number(VM[i][VM[0].size()-1].real).toInt()-1][col].real = QString::fromStdString(elseVal).toDouble();
                }
            }
            else{
                if(c<index.size()){
                    if((int) i != index[c].real){
                        VMreal[QString::number(VM[i][VM[0].size()-1].real).toInt()-1][col].string = elseVal;
                    }else{
                        c++;
                    }
                }else{
                    VMreal[QString::number(VM[i][VM[0].size()-1].real).toInt()-1][col].string = elseVal;
                }
            }
        }
    }
}

std::vector<type> DMRecodeDialog::mergesort1(std::vector<type> &vec, int threads, bool order)
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
    }
    */
}

std::vector<type> DMRecodeDialog::merge1(const std::vector<type> &left, const std::vector<type> &right, bool order)
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
