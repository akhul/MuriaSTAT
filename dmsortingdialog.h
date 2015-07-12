#ifndef DMSORTINGDIALOG_H
#define DMSORTINGDIALOG_H

#include <QDialog>
//#include <omp.h>  // Untuk parallel OpenMP, copy ini ke .pro --> QMAKE_CXXFLAGS += -fopenmp QMAKE_LFLAGS += -fopenmp
#include <RInside.h>
#include <QtGui>
#include "datamanagement.h"
#include "dataeditor.h"
#include "activedataset.h"

class DataEditor;
class ActiveDataset;

namespace Ui {
class DMSortingDialog;
}

class DMSortingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DMSortingDialog(QWidget *parent, RInside &m_R);
    ~DMSortingDialog();
    void setDE(DataEditor *de);
    void setCBDatasetName(QStringList dataSetList);
    void setCurrentCBDSName(int index);
    void setDataDisplayedName(QString name);
    void setActiveDS(ActiveDataset *ads);

public slots:

private slots:
    void sort();
    void setSelectedRB1(const QString &);
    void setSelectedRB2(const QString &);
    void selectVar();
    void selectDataset(int i);

private:
    Ui::DMSortingDialog *ui;
    DataEditor *dataEditor;
    ActiveDataset *activeDS;
    QStringList varTypes;
    QStringList varNames;
    std::vector<bool> order;
    RInside m_R;
    QString dataDisplayedName;
    void moveCurrentItem(QListWidget *source, QListWidget *target);

};

#endif // DMSORTINGDIALOG_H
