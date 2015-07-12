#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "RInside.h"
#include <QtGui>
#include "activedataset.h"
#include "open.h"
#include "rcore.h"
#include "tabeditor.h"
#include "dataeditor.h"
#include "variableeditor.h"
//Update 27 Juni
#include "save.h"

#include "qwebview.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent, RInside &R);
    ~MainWindow();
    //Update 26 Juni
    void setCurrentFile(const QString &fileName2); //sebelumnya private
    void setDataEditorConnect(DataEditor *DE);

    //Update 30 Juni
    void disableActionSave();
    void showSaveDialog(QString dataName);
    void saveToR();
    void addNewVarVE(int col, QString newVar, QString newType);
    void deleteVarVE(int col);
    void deleteRow(int rowCount);
    void addNewRow();
    void insertVarVE(int col);

private:
    //method
    void createContentMainWindow();
    void intialActiveDataset();
    Ui::MainWindow *ui;
    RInside &R;
    QSplitter* vSplitter1;
    QSplitter* vSplitter2;
    QSplitter* vSplitterMain;
    QSplitter* hSplitter;
    QPushButton* openButtonAD;
    QString fileName;
    QString strippedName(const QString &fullFileName);
    ActiveDataset* activeDatasetClass;
    Open* openClass;
    RCore* rCoreClass;
    TabEditor* tabEditorClass;

    DataEditor* dataEditor;
    VariableEditor* variableEditor;
    //update 27 juni
    void setOutputDesign();
    void showMainWidget(int i); //0=PlainText, 1=Editor, 2=Output
    Save *save;
    //Update 30 Juni
    QTreeWidget *treeOutput;
    QWebView *outputWV;
    void addOutput(QString);
    void clearOutput();
    void addTreeOutputItem(QString dataset, QString analysis, QStringList componentList);
    void printToPDF();

public slots:
    //update 27 Juni

private slots:
    void createActions();
    void setWindowShow();
    void openNewData();
    //==update==//
    void SlRegressionDialog();
    //Update 27 juni
    void newData();
    void saveFile();
    void saveAs();
    void enableActionSave();
    void sort();
    //Update 29 Juni
    void scrollOutput();
    void recodeSameVar();
    void aggregate();
    void mergeByCol();
    void exit();

};
#endif // MAINWINDOW_H
