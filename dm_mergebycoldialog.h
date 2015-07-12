#ifndef DM_MERGEBYCOLDIALOG_H
#define DM_MERGEBYCOLDIALOG_H

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
class DM_MergeByColDialog;
}

class DM_MergeByColDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DM_MergeByColDialog(QWidget *parent, RInside &m_R);
    ~DM_MergeByColDialog();
    void setDE(DataEditor *de);
    void setCBDatasetName(QStringList dataSetList);
    void setDataDisplayedName(QString name);
    void setActiveDS(ActiveDataset *ads);

private slots:
    void megreByCol();
    void selectDataset1(int i);
    void selectDataset2(int i);

private:
    Ui::DM_MergeByColDialog *ui;
    DataEditor *dataEditor;
    QStringList varTypes;
    QStringList varNames;
    RInside m_R;
    QString dataDisplayedName;
    ActiveDataset *activeDS;
    std::vector<std::vector<type> > mergeCol1(std::vector<std::vector<type> > VM1, std::vector<std::vector<type> > VM2, std::vector<int> cols1, std::vector<int> cols2);

};

#endif // DM_MERGEBYCOLDIALOG_H
