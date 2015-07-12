#include "dm_mergebycoldialog.h"
#include "ui_dm_mergebycoldialog.h"
#include "mainwindow.h"

DM_MergeByColDialog::DM_MergeByColDialog(QWidget *parent, RInside &m_R) :
    QDialog(parent),
    ui(new Ui::DM_MergeByColDialog),
    m_R(m_R)
{
    ui->setupUi(this);
    connect(this,SIGNAL(accepted()),this,SLOT(megreByCol()));
}

DM_MergeByColDialog::~DM_MergeByColDialog()
{
    delete ui;
}

void DM_MergeByColDialog::setDataDisplayedName(QString name)
{
    dataDisplayedName = name;
}

void DM_MergeByColDialog::setDE(DataEditor *de)
{
    dataEditor = de;
}

void DM_MergeByColDialog::setCBDatasetName(QStringList list)
{
    ui->dataset1_CB->clear();
    ui->dataset1_CB->addItems(list);
    ui->dataset2_CB->clear();
    ui->dataset2_CB->addItems(list);
    connect(ui->dataset1_CB,SIGNAL(currentIndexChanged(int)),this,SLOT(selectDataset1(int)));
    connect(ui->dataset2_CB,SIGNAL(currentIndexChanged(int)),this,SLOT(selectDataset2(int)));
    selectDataset1(0);
    selectDataset2(0);
}

void DM_MergeByColDialog::setActiveDS(ActiveDataset *ads1)
{
    activeDS = ads1;
}

void DM_MergeByColDialog::selectDataset1(int index)
{
    Rcpp::CharacterVector varNames1 = m_R.parseEval("colnames("+ui->dataset1_CB->itemText(index).toStdString()+")");
    ui->colID1_CB->clear();
    for(int i=0; i< varNames1.size(); i++){
        ui->colID1_CB->addItem(QString(varNames1[i]));
    }
}

void DM_MergeByColDialog::selectDataset2(int index)
{
    Rcpp::CharacterVector varNames1 = m_R.parseEval("colnames("+ui->dataset2_CB->itemText(index).toStdString()+")");
    ui->colID2_CB->clear();
    for(int i=0; i< varNames1.size(); i++){
        ui->colID2_CB->addItem(QString(varNames1[i]));
    }
}

void DM_MergeByColDialog::megreByCol()
{
    /*qDebug("------MERGE COL "+ui->dataset1_CB->currentText().toLocal8Bit()+" and "+ui->dataset2_CB->currentText().toLocal8Bit()+"-----");
    Rcpp::DataFrame DF1 = m_R.parseEval(ui->dataset1_CB->currentText().toStdString());
    Rcpp::DataFrame DF2 = m_R.parseEval(ui->dataset2_CB->currentText().toStdString());

    //Index merge column(s)
    std::vector<int> cols1,cols2;
    QStringList types1, types2;
    cols1.push_back(ui->colID1_CB->currentIndex());
    cols2.push_back(ui->colID2_CB->currentIndex());

    std::vector<bool> order;
    order.push_back(true);
    double vt = omp_get_wtime();
    varNames.clear();
    varTypes.clear();
    DataManagement *dm = new DataManagement;
    QStringList varTypes1 = dm->getVarTypesFromDF(DF1);
    QStringList varTypes2 = dm->getVarTypesFromDF(DF2);
    types1.push_back(varTypes1[cols1[0]]);
    types2.push_back(varTypes2[cols2[0]]);
    for(int k =0; k < (ui->colID1_CB->count()); k++){
        varNames.push_back(ui->colID1_CB->itemText(k));
        varTypes.push_back(varTypes1[k]);
    }
    for(int k =0; k < (ui->colID2_CB->count()); k++){
        for(unsigned c=0; c<cols2.size(); c++){
            if(k != (int) cols2[c]){
                varNames.push_back(ui->colID2_CB->itemText(k));
                varTypes.push_back(varTypes2[k]);
            }
        }
    }
    vt = omp_get_wtime() -vt;
    qDebug("Set varTypes");
    qDebug() << vt;
    //newMatrix
    std::vector<std::vector<type> > VMResult;
    double ttot = omp_get_wtime();
    double tsort1 = omp_get_wtime();
    //conversion
    //std::vector<std::vector<type> > VM1 = DFToV2RowType(DF1);
    double tc1 = omp_get_wtime();
    Rcpp::CharacterVector charVector = DF1[0];
    std::vector<std::vector<type> > VM1(charVector.size(), std::vector<type>(DF1.size()));
    for(int j=0;j<DF1.size();j++)
    {
        if(varTypes1.at(j)=="String"){
            charVector = DF1[j];
            for(int i=0;i<charVector.size();i++){
                VM1[i][j].string = charVector[i];
            }
        }else{
            Rcpp::NumericVector numVector = DF1[j];
            for(int i=0;i<numVector.size();i++){
                VM1[i][j].real = numVector[i];
            }
        }
    }
    //std::vector<std::vector<type> > VM2 = DFToV2RowType(DF2);
    charVector = DF2[0];
    std::vector<std::vector<type> > VM2(charVector.size(), std::vector<type>(DF2.size()));
    for(int j=0;j<DF2.size();j++)
    {
        if(varTypes2.at(j)=="String"){
            charVector = DF2[j];
            for(int i=0;i<charVector.size();i++){
                VM2[i][j].string = charVector[i];
            }
        }else{
            Rcpp::NumericVector numVector = DF2[j];
            for(int i=0;i<numVector.size();i++){
                VM2[i][j].real = numVector[i];
            }
        }
    }
    tc1 = omp_get_wtime() -tc1;
    qDebug("DF to V");
    qDebug() << tc1;
    //sorting kedua dataset berdasarkan mergeColumn
    //VM1 = mergesort(VM1, 1, cols1, order);
    //VM2 = mergesort(VM2, 1, cols2, order);
    dm->mergeSort(VM1,cols1,order,types1,1);
    dm->mergeSort(VM2,cols2,order,types2,1);
    tsort1 = omp_get_wtime() -tsort1;
    qDebug("Sort ser");
    qDebug() << tsort1;

    //MergeCol Serial
    double t2 = omp_get_wtime();
    *//*for(unsigned i = 0; i < VM1.size(); i++){
        for(unsigned j = 0; j < VM2.size(); j++){
            bool same=false;
            for(unsigned c=0; c<cols1.size(); c++){
                if(VM1[i][cols1[c]].real == VM2[j][cols2[c]].real){
                    same=true;
                }else{
                    same=false;
                    break;
                }
            }
            if(same){
                std::vector<type> V;
                for(unsigned k =0; k < VM1[0].size(); k++){
                    V.push_back(VM1[i][k]);
                }
                for(unsigned k =cols2.size(); k < VM2[0].size(); k++){
                    V.push_back(VM2[j][k]);
                }
                VMResult.push_back(V);
            }
            else if(VM1[i][cols1[0]].real < VM2[j][cols2[0]].real){
                break;
            }
        }
    }*/
    /*
    VMResult = mergeCol1(VM1, VM2,cols1, cols2);
    double timeSerial = (omp_get_wtime()-t2);
    qDebug("Serial");
    qDebug() << timeSerial;
    //VMResult.clear();

    //----------------PARALLEL 4 Core
    double tsort2 = omp_get_wtime();
    //conversion
    charVector = DF1[0];
    //std::vector<std::vector<type> > VM1(charVector.size(), std::vector<type>(DF1.size()));
    for(int j=0;j<DF1.size();j++)
    {
        if(varTypes1.at(j)=="String"){
            charVector = DF1[j];
            for(int i=0;i<charVector.size();i++){
                VM1[i][j].string = charVector[i];
            }
        }else{
            Rcpp::NumericVector numVector = DF1[j];
            for(int i=0;i<numVector.size();i++){
                VM1[i][j].real = numVector[i];
            }
        }
    }
    //std::vector<std::vector<type> > VM2 = DFToV2RowType(DF2);
    charVector = DF2[0];
    //std::vector<std::vector<type> > VM2(charVector.size(), std::vector<type>(DF2.size()));
    for(int j=0;j<DF2.size();j++)
    {
        if(varTypes2.at(j)=="String"){
            charVector = DF2[j];
            for(int i=0;i<charVector.size();i++){
                VM2[i][j].string = charVector[i];
            }
        }else{
            Rcpp::NumericVector numVector = DF2[j];
            for(int i=0;i<numVector.size();i++){
                VM2[i][j].real = numVector[i];
            }
        }
    }

    //sorting kedua dataset berdasarkan mergeColumn
    //VM1 = mergesort(VM1, 4, cols1, order);
    //VM2 = mergesort(VM2, 4, cols2, order);
    dm->mergeSort(VM1,cols1,order,types1,4);
    dm->mergeSort(VM2,cols2,order,types2,4);
    tsort2 = omp_get_wtime() -tsort2;
    qDebug("Sort Par 4");
    qDebug() << tsort2;

    double tpar = omp_get_wtime();
    std::vector<std::vector<type> > VM2a, VM2b, VM2c, VM2d;
    for(unsigned i = 0; i< VM2.size(); i++){
        if(i < VM2.size()/4){
            VM2a.push_back(VM2[i]);
        }else if(i>VM2.size()/4 && i < VM2.size()/2){
            VM2b.push_back(VM2[i]);
        }else if(i>VM2.size()/2 && i < VM2.size()*3/4){
            VM2c.push_back(VM2[i]);
        }else{
            VM2d.push_back(VM2[i]);
        }
    }
    std::vector<std::vector<type> > VMResult1, VMResult2, VMResult3, VMResult4;
    double tpar1 = omp_get_wtime();
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            VMResult1 = mergeCol1(VM1, VM2a,cols1, cols2);
        }
        #pragma omp section
        {
            VMResult2 = mergeCol1(VM1, VM2b,cols1, cols2);
        }
        #pragma omp section
        {
            VMResult3 = mergeCol1(VM1, VM2c,cols1, cols2);
        }
        #pragma omp section
        {
            VMResult4 = mergeCol1(VM1, VM2d,cols1, cols2);
        }
    }
    double tpragma = omp_get_wtime() - tpar1;
    qDebug("Par4 pragma");
    qDebug() << tpragma;
    //combine
    for(unsigned i = 0; i< VMResult2.size(); i++){
        VMResult1.push_back(VMResult2[i]);
    }
    for(unsigned i = 0; i< VMResult3.size(); i++){
        VMResult1.push_back(VMResult3[i]);
    }
    for(unsigned i = 0; i< VMResult4.size(); i++){
        VMResult1.push_back(VMResult4[i]);
    }
    double tcombine = omp_get_wtime() - tpar1;
    qDebug("Par4 + combine");
    qDebug() << tcombine;

    //VMResult1 = mergesort(VMResult1,4,cols1,order);
    //double tsortf = omp_get_wtime() - tpar1;
    //qDebug("Par4 + combine + sort");
    //qDebug() << tsortf;

    tpar = omp_get_wtime() - tpar;
    qDebug("Par4");
    qDebug() << tpar;
    //conversion
    //DF1 = V2RowTypetoDF(VMResult1);
    double tc2 = omp_get_wtime();
    Rcpp::DataFrame DF;
    for(unsigned j =0; j< VMResult1[0].size();j++ ){
        if(varTypes.at(j)=="String"){
            std::vector<std::string> V;
            for(unsigned i=0; i< VMResult1.size(); i++){
                V.push_back(VMResult1[i][j].string);
            }
            DF.push_back(V);
        }
        else if(varTypes.at(j)=="Integer"){
            std::vector<int> Vi;
            for(unsigned i=0; i< VMResult1.size(); i++){
                Vi.push_back(VMResult1[i][j].real);
            }
            DF.push_back(Vi);
        }else{
            std::vector<double> Vd;
            for(unsigned i=0; i< VMResult1.size(); i++){
                Vd.push_back(VMResult1[i][j].real);
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
    DF = call.eval();
    tc2 = omp_get_wtime()-tc2;
    qDebug("V to DF");
    qDebug() << tc2;
    QString name;
    if(ui->newDatasetName->text().isEmpty()){
        name = "Merge_"+ui->dataset1_CB->currentText()+"_"+ui->dataset2_CB->currentText();
    }else{
        name = ui->newDatasetName->text();
    }
    m_R[name.toStdString()] = DF;
    ttot = omp_get_wtime() - ttot;
    //QMessageBox::information(this, "Time", "Parallel: "+QString::number(tpar) +"Convert&Sort: "+QString::number(tsort2)+"\nSerial: "+ QString::number(timeSerial)+"Convert&Sort: "+QString::number(tsort1)+"\nTime Total: "+QString::number(ttot));
    //QMessageBox::information(0,"Dim","Row: "+QString::number(VMResult1.size())+"Col: "+QString::number(VMResult1[0].size()));
    double tr = omp_get_wtime();
    //add new dataset
    int row = VMResult1.size();
    int col = VMResult1[0].size();
    QString ext ="", path ="";
    activeDS->addRowData(colNames,name,ext,path,col,row);

    //Load table
    activeDS->getVarEditor()->setDataEditor(dataEditor);
    activeDS->getVarEditor()->setDatasetName(name);
    activeDS->getVarEditor()->reloadVar(varNames, varTypes);
    dataEditor->setVM(VMResult1);
    dataEditor->reloadTable(true,false);
    //dataEditor->clearIdxChanged();
    //activeDS->reloadData(QString::fromStdString(name));
    tr = omp_get_wtime() - tr;
    qDebug("reload tabel");
    qDebug() << tr;
    QMessageBox::information(0,"Reload tabel","Reload tabel: "+QString::number(tr));
    //MainWindow *mw = (MainWindow*) QApplication::activeWindow();
    //mw->setCurrentFile(name);
    delete dm;
*/
}

std::vector<std::vector<type> > DM_MergeByColDialog::mergeCol1(std::vector<std::vector<type> > VM1, std::vector<std::vector<type> > VM2, std::vector<int> cols1, std::vector<int> cols2)
{
    //Belum ada validasi type, masih real(double) saja
    std::vector<std::vector<type> > VMResult1;
    for(unsigned i = 0; i < VM1.size(); i++){
        for(unsigned j = 0; j < VM2.size(); j++){
            bool same=false;
            for(unsigned c=0; c<cols1.size(); c++){
                if(VM1[i][cols1[c]].real == VM2[j][cols2[c]].real){
                    same=true;
                }else{
                    same=false;
                    break;
                }
            }
            if(same){
                std::vector<type> V;
                for(unsigned k =0; k < VM1[0].size(); k++){
                    V.push_back(VM1[i][k]);
                }
                for(unsigned k =cols2.size(); k < VM2[0].size(); k++){
                    V.push_back(VM2[j][k]);
                }
                VMResult1.push_back(V);
            }
            else if(VM1[i][cols1[0]].real < VM2[j][cols2[0]].real){
                break;
            }
        }
    }
    return VMResult1;
}
