#ifndef DMRECODEDIALOG_H
#define DMRECODEDIALOG_H

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
class DMRecodeDialog;
}

class DMRecodeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DMRecodeDialog(QWidget *parent, RInside &m_R);
    ~DMRecodeDialog();
    void setDE(DataEditor *de);
    void setCBDatasetName(QStringList dataSetList);
    void setCurrentCBDSName(int index);
    void setDataDisplayedName(QString name);
    void setActiveDS(ActiveDataset *ads);

private slots:
    void selectVar();
    void selectDataset(int i);
    void selectTypeVal();
    void addVal();
    void removeVal();
    void recodeSameVar();
    void elseVal();
    void renameVar();

private:
    Ui::DMRecodeDialog *ui;
    DataEditor *dataEditor;
    ActiveDataset *activeDS;
    QStringList varTypes;
    QStringList varNames;
    //std::vector<int> cols;
    //std::vector<bool> order;
    RInside m_R;
    QStringList oldVal, oldValLow, oldValHigh, newVal;
    QString dataDisplayedName;
    void moveCurrentItem(QListWidget *source, QListWidget *target);
    void bsearch_recode(std::vector<std::vector<type> >& VM,std::vector<std::vector<type> >& VMreal, int low, int high,int col, QStringList oldVal, QStringList newVal);
    void bsr_interval(std::vector<std::vector<type> > &VM, std::vector<std::vector<type> > &VMreal, int low, int high, int col, QStringList oldVal1, QStringList oldVal2, QStringList newVal);
    QString checkVarType(QStringList V);
    std::vector<type> merge1(const std::vector<type>& left, const std::vector<type>& right, bool order);
    std::vector<type> mergesort1(std::vector<type>& vec, int threads, bool order);


};

#endif // DMRECODEDIALOG_H
