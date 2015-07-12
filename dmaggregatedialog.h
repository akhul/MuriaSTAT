#ifndef DMAGGREGATEDIALOG_H
#define DMAGGREGATEDIALOG_H

#include <QDialog>
//#include <omp.h>
#include <RInside.h>
#include <QtGui>
#include "datamanagement.h"
#include "dataeditor.h"
#include "activedataset.h"

class DataEditor;
class ActiveDataset;

namespace Ui {
class DMAggregateDialog;
}

class DMAggregateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DMAggregateDialog(QWidget *parent, RInside &m_R);
    ~DMAggregateDialog();
    void setDE(DataEditor *de);
    void setCBDatasetName(QStringList dataSetList);
    void setCurrentCBDSName(int index);
    void setDataDisplayedName(QString name);
    void setActiveDS(ActiveDataset *ads);

private slots:
    void aggregate();
    void selectVarID();
    void selectVarTarget();
    void selectDataset(int i);

private:
    Ui::DMAggregateDialog *ui;
    DataEditor *dataEditor;
    QStringList varTypes;
    QStringList varNames;
    RInside m_R;
    QString dataDisplayedName;
    ActiveDataset *activeDS;
    void moveCurrentItem(QListWidget *source, QListWidget *target);
    std::vector<std::vector<std::vector<type> > > aggregate1(std::vector<std::vector<type> >VM,int low, int high, std::vector<int> colID, std::vector<int> colTarget);
    std::vector<type> merge1(const std::vector<type>& left, const std::vector<type>& right, bool order);
    std::vector<type> mergesort1(std::vector<type>& vec, int threads, bool order);

};

#endif // DMAGGREGATEDIALOG_H
