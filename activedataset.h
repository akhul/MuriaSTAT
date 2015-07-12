#ifndef ACTIVEDATASET_H
#define ACTIVEDATASET_H

#include <QWidget>
#include <QtGui>
#include <rcore.h>
#include "buttoncustom.h"
#include "tabeditor.h"

class MainWindow;

class ActiveDataset : public QTableWidget

{
    Q_OBJECT
public:
    explicit ActiveDataset(QWidget *parent, DataEditor *dataEditor2, VariableEditor *variableEditor2);
    ~ActiveDataset();
    void addRowData(Rcpp::StringVector colnames, QString name, QString ext, QString path, int col, int row); //Update 27 juni, tambah extension dan path
    void setRcore(RCore* r);
    //==update==//
    QStringList getDatasetNameList();
    QStringList getVarList();

    //update 27 juni
    void setMainEditor(QWidget* w);
    void setMainText(QPlainTextEdit* w);
    void setMainOutput(QWidget* w);
    void updateAllList(QString oldName, QString newName, QString newExt, QString newPath);
    DataEditor* getDataEditor();
    QStringList getExtList();
    QStringList getPathList();
    void reloadData(QString dataName);
    void setDataEditor(DataEditor *de);
    void setVarEditor(VariableEditor *ve);
    VariableEditor* getVarEditor();

    //Update 30 Juni
    void deleteDataset(int row);
    void updateRow(int index, Rcpp::StringVector colNames, QString oldName, QString newName, QString ext, QString path, int col, int row);
    void updateDSModifiedList(int index);
    bool getDSModifiedAt(int index);
    int getDatasetIndex(QString datasetName);
    int getDatasetRowIndex(QString datasetName);

private:
    //method//
    void registerSlot(QPushButton *button,int index);
    void registerSlotHide(QPushButton *button,int start);
    void registerSlotChange(ButtonCustom *button,int start);
    //==Update==//
    void setDatasetName(QString datasetName);
    void setVarName(QString VarName);
    //variable//
    QSignalMapper *signalMapper;
    QSignalMapper *signalMapper2;
    QPlainTextEdit* mainPlainText;
    QWidget* mainEditor;
    QWidget* mainOutput;
    RCore* rCore;
    int numberTable;
    int start;
    int end;
    QString statusButton;
    DataEditor *dataEditor;
    VariableEditor *variableEditor;
    //==Update==//
    QStringList datasetNameList;
    QStringList varList;

    //Update 26 Juni
    MainWindow *mainWindow;
    QStringList extensionList;
    QStringList pathList;
    QList<bool> dataSetModifiedList;

signals:

private slots:
    void activeLoad(int index);
    void activeHideExpand(int index);
    void changeButton(QPushButton* button, int index);

    //Update 27 Juni
    void showContextMenu(const QPoint&);

};

#endif // ACTIVEDATASET_H
