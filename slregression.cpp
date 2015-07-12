#include "slregression.h"
#include "ui_slregression.h"
#include <qstandarditemmodel.h>
#include <qlistview.h>
#include <qstringlistmodel.h>
#include <qmessagebox.h>

SLRegression::SLRegression(QWidget *parent, RInside &m_R) :
    QDialog(parent),
    ui(new Ui::SLRegression),
    R(m_R)
{
    ui->setupUi(this);
    select =ui->tableView->selectionModel();
    connect(ui->comboBox,SIGNAL(currentIndexChanged(QString)),this,SLOT(viewVar(QString)));
    connect(ui->tableView,SIGNAL(clicked(QModelIndex)),this,SLOT(valVarTable()));
    connect(ui->independenListView,SIGNAL(clicked(QModelIndex)),this,SLOT(valIndependenList()));
    connect(ui->dependenListView,SIGNAL(clicked(QModelIndex)),this,SLOT(valDependentList()));

   ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
   ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
   ui->tableView->horizontalHeader()->setHighlightSections(false);



   //first's design  variable table
   model1=new QStandardItemModel(0,2,this);
   item1 = new QStandardItem;
   item2 = new QStandardItem;
   item1->setText(QString("Variables"));
   item1->setTextAlignment(Qt::AlignLeft);
   item2->setText(QString("Type"));
   item2->setTextAlignment(Qt::AlignLeft);

   model1->setHorizontalHeaderItem(0,item1);
   model1->setHorizontalHeaderItem(1,item2);

   ui->tableView->setModel(model1);
   ui->tableView->setColumnWidth(0,155);
   ui->tableView->setColumnWidth(1,155);


   QString styleSheet = "::section {" // "QHeaderView::section {"
                        "spacing: 12px;"
                        "background-color: SkyBlue;"
                        "color: black;"
                        "margin: 3px 2px 3px 2px;"
                        "text-align: right;}";

   ui->tableView->horizontalHeader()->setStyleSheet(styleSheet); // tableWidget->verticalHeader()->setStyleSheet(styleSheet);
  // ui->tableView->verticalHeader()->setFixedWidth(25);
   ui->tableView->verticalHeader()->hide();


}

SLRegression::~SLRegression()
{
    delete ui;
}
void SLRegression::setCBDatasetName(QStringList list){

    ui->comboBox->addItems(list);
}

void SLRegression::setR(RInside &m_R){
    R=m_R;
}
void SLRegression::viewVar(QString datasetName){
    int x=QString::compare(datasetName,"No dataset selected...",Qt::CaseInsensitive);
    if(x!=0){

        std::string dataset = datasetName.toStdString();
        std::string syntax = "varType<-as.vector(sapply("+dataset+",class));varMat<-cbind(colnames("+dataset+"),varType)";
        //varType<-as.vector(sapply(psych,class));varMat<-cbind(colnames(psych),varType)
        R.parseEvalQ(syntax);
        Rcpp::DataFrame varMatrix=R.parseEval("varMat");
                R.parseEvalQ("dimension<-dim(varMat)");
                int row = R.parseEval("dimension[1]");
        //int row =6;

        QStandardItemModel *model=new QStandardItemModel(row,2,this);
        QStandardItem *item1 = new QStandardItem;
        QStandardItem *item2 = new QStandardItem;
        item1->setText(QString("Variables"));
        item1->setTextAlignment(Qt::AlignLeft);
        item2->setText(QString("Type"));
        item2->setTextAlignment(Qt::AlignLeft);

        model->setHorizontalHeaderItem(0,item1);
        model->setHorizontalHeaderItem(1,item2);


        for (int i=0; i<2; i++) {
            //QTableWidgetItem *item = new QTableWidgetItem;
            //item->setText(QString(rnames[i]));
            //tableModel->setVerticalHeaderItem(i, item);
            Rcpp::CharacterVector x = varMatrix[i];
                for (int j=0; j<row; j++) {
                    QStandardItem *item = new QStandardItem;
                    item->setText(QString(x[j]));
                    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                  //  item->setTextAlignment(Qt::AlignCenter);
                    model->setItem(j,i,item);
                }

            }
        ui->tableView->setModel(model);
        ui->tableView->setColumnWidth(1,155);
        ui->tableView->setColumnWidth(0,154);


        QString styleSheet = "::section {" // "QHeaderView::section {"
                             "spacing: 12px;"
                             "background-color: SkyBlue;"
                             "color: black;"
                             "margin: 3px 2px 3px 2px;"
                             "text-align: right;}";

        ui->tableView->horizontalHeader()->setStyleSheet(styleSheet); // tableWidget->verticalHeader()->setStyleSheet(styleSheet);
       // ui->tableView->verticalHeader()->setFixedWidth(25);
        ui->tableView->verticalHeader()->hide();
    }
    else{

        ui->tableView->setModel(model1);
    }

}

void SLRegression::on_pushButton_clicked()
{
    int row = ui->tableView->selectionModel()->currentIndex().row();
    if(row>-1){
        int textBtn = QString::compare(ui->pushButton->text(),"<<",Qt::CaseInsensitive);
        if(textBtn!=0){
            QString value = ui->tableView->model()->data(ui->tableView->model()->index(row,0)).toString();
            QStringListModel *model = new QStringListModel(this);
            //QString value1=QString::number(row);
            dependentList<<value;
            model->setStringList(dependentList);
            ui->dependenListView->setModel(model);
        }
        else{
            QString value = ui->dependenListView->model()->data(ui->dependenListView->model()->index(0,0)).toString();
            dependentList.removeAll(value);
            //QString value1=QString::number(dependentList.size());
           // dependentList<<value1;
            QStringListModel *model = new QStringListModel(this);
            model->setStringList(dependentList);
            ui->dependenListView->setModel(model);


        }
    }

}

void SLRegression::on_pushButton_2_clicked()
{
    int row = ui->tableView->selectionModel()->currentIndex().row();
    if(row>-1){
        int textBtn = QString::compare(ui->pushButton_2->text(),"<<",Qt::CaseInsensitive);
        if(textBtn!=0){
            QString value = ui->tableView->model()->data(ui->tableView->model()->index(row,0)).toString();
            QStringList list = independentList.filter(value, Qt::CaseInsensitive);
                if(list.size()<=0){
                    QStringListModel *model = new QStringListModel(this);
                    //QString value1=QString::number(row);
                    independentList<<value;
                    model->setStringList(independentList);
                    ui->independenListView->setModel(model);

                    int index = independentList.size()-1;
                    QModelIndex indexs = ui->independenListView->model()->index(index,0);
                    ui->independenListView->setCurrentIndex(indexs);
                    ui->independenListView->selectionModel()->select(indexs,QItemSelectionModel::Select);
                    ui->independenListView->setFocus();


                }
                else{
                    int index = independentList.indexOf(QRegExp(value,Qt::CaseInsensitive));
                    QModelIndex indexs = ui->independenListView->model()->index(index,0);

                    ui->independenListView->setCurrentIndex(indexs);
                    ui->independenListView->selectionModel()->select(indexs,QItemSelectionModel::Select);
                    ui->independenListView->setFocus();


                }


        }
        else{
            int row = ui->independenListView->selectionModel()->currentIndex().row();
            QString value = ui->independenListView->model()->data(ui->independenListView->model()->index(row,0)).toString();
            independentList.removeAll(value);
            //QString value1=QString::number(dependentList.size());
           // dependentList<<value1;
            QStringListModel *model = new QStringListModel(this);
            model->setStringList(independentList);
            ui->independenListView->setModel(model);

        }

    }
}

void SLRegression::valVarTable(){
    ui->pushButton->setText(">>");
    ui->pushButton_2->setText(">>");
    ui->independenListView->clearSelection();
    ui->dependenListView->clearSelection();

    int row = dependentList.size();
    if(row>0){
      ui->pushButton->setEnabled(false);
    }
//ui->pushButton->setEnabled(false);


}
void SLRegression::valDependentList(){
    ui->tableView->clearSelection();
    int length = dependentList.size();
    if(length>-1){
        ui->pushButton->setEnabled(true);
        ui->pushButton->setText("<<");
    }

}
void SLRegression::valIndependenList(){
    ui->tableView->clearSelection();
    int length = independentList.size();
    if(length>-1){
        //ui->pushButton_2->setEnabled(true);
        ui->pushButton_2->setText("<<");
    }
}

std::string SLRegression::getDatasetName(){
    return ui->comboBox->currentText().toStdString();
}
std::string SLRegression::getModel(){

    return model;
}
QStringList SLRegression::getStatIndicator(){

    return statIndicator;
}




void SLRegression::on_buttonBox_accepted()
{
    std::string indepModel = independentList.join("+").toStdString();
    model = dependentList[0].toStdString()+"~"+indepModel;
    QMessageBox::information(this,"","test");
    if(ui->cofficientsCB->isChecked()){
        //coefficients
        statIndicator.append("coefficients");
    }
    if(ui->confidentICB->isChecked()){
        //ci
        statIndicator.append("ci");
    }
    if(ui->anovaCB->isChecked()){
        //anova
        statIndicator.append("anova");
    }
    if(ui->covMatCB->isChecked()){
        //covarians
        statIndicator.append("covarians");
    }
    if(ui->modelCB->isChecked()){
        //model
        statIndicator.append("model");
    }
    if(ui->resDurbinCB->isChecked()){
        //residualDurbin
        statIndicator.append("residual");
    }

}
