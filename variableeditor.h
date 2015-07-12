#ifndef VARIABLEEDITOR_H
#define VARIABLEEDITOR_H

#include <QTableWidget>
#include <QtGui>
#include <RInside.h>
#include "rcore.h"
#include "dataeditor.h"

class ActiveDataset; //Update

class VariableEditor : public QTableWidget
{
    Q_OBJECT
public:
    explicit VariableEditor(QWidget *parent = 0);
    ~VariableEditor();
    void loadVariable(RCore *rExe, Rcpp::StringVector colNames);
    //--Update
    void setDataEditor(DataEditor *dataEditor);
    void setActiveDataset(ActiveDataset *activeDataset);
    //Update 30 Juni
    void reloadVar(QStringList varNames,QStringList varTypes);
    void setDatasetName(QString name);
    void addNewVarVE(int col, QString newVar, QString newType);
    void deleteVarVE(int col);

private:
    QStringList varNamesVE; //Update
    void keyPressEvent(QKeyEvent* event);
    void addNewVar(int index, int j);
    void setRowTable(int rowCount, int start);
    void setRExe(RCore *rExe);
    RCore *rExe;
    //--Update
    DataEditor *dataEditor;
    ActiveDataset *activeDataset;
    int rowBefore, colBefore;
    QStringList varTypesVE;
    //Update 1 Juli
    QString datasetName;

public slots:
    //update 30Juni
    void checkWidgetVisibility();

private slots:
    void changeVarProp(int i, int j);
    //--Update
    void decimalChanged(const QString &);
    void comboChanged(const QString &);
    void cellWidgetClicked(int i, int j);
    void showRowContextMenu(const QPoint&);

};

#endif // VARIABLEEDITOR_H
