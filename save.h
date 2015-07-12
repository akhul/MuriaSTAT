#ifndef SAVE_H
#define SAVE_H

#include <QtGui>
#include <RInside.h>
#include "dataeditor.h"
#include "activedataset.h"
#include "rcore.h"

class DataEditor;
class ActiveDataset;
class RCore;

class Save
{
public:
    Save(RInside &R);
    ~Save();

    void setRCoreClass(RCore *rc);
    void setActiveDataSet(ActiveDataset *ads);
    void setMainWindow(MainWindow *mw);

    void saveToR();
    void saveDatasetToFile();

public slots:
    void saveFile();
    void saveAs();

private:
    DataEditor *dataEditor;
    ActiveDataset *activeDatasetClass;
    MainWindow *mainWindow;
    RInside &R;
    RCore *rCore;
};

#endif // SAVE_H
