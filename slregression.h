#ifndef SLREGRESSION_H
#define SLREGRESSION_H

#include <QDialog>
#include "RInside.h"
#include <QStandardItem>
#include <QItemSelectionModel>

namespace Ui {
class SLRegression;
}

class SLRegression : public QDialog
{
    Q_OBJECT

public:
    explicit SLRegression(QWidget *parent,RInside &m_R);
    ~SLRegression();
    void setCBDatasetName(QStringList datasetNameList);
    void setR(RInside &m_R);
    std::string getDatasetName();
    QStringList getStatIndicator();
    std::string getModel();

private:
    Ui::SLRegression *ui;
    RInside &R;
    QStandardItemModel *model1;
    QStandardItem *item1;
    QStandardItem *item2;
    QItemSelectionModel *select;
    QStringList dependentList;
    QStringList independentList;
    std::string model;
    std::string datasetname;
    QStringList statIndicator;


private slots:
    void viewVar(QString datasetName);
    void valVarTable();
    void valDependentList();
    void valIndependenList();


    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_buttonBox_accepted();
};

#endif // SLREGRESSION_H
