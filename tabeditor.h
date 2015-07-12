#ifndef TABEDITOR_H
#define TABEDITOR_H

#include <QWidget>
#include <QtGui>
#include "rcore.h"
#include "dataeditor.h"
#include "variableeditor.h"

class TabEditor : public QTabWidget
{
    Q_OBJECT
public:
    explicit TabEditor(QWidget *parent = 0);
    ~TabEditor();
    QString getPath();
    QString getFileName();
    QString getExtension();
    DataEditor *getDataEdit();
    void setPath(QString path);
    void setFileName(QString fileName);
    void setExtension(QString extension);
    void openFileDialog();
    void openEditor(RCore *rExe, DataEditor *dataEditor, VariableEditor *variableEditor);
    void displayEditor(RCore *rExe, std::string name, DataEditor *dataEditor, VariableEditor *variableEditor);
    //update 27 Juni
    void newEditor(DataEditor *dataEditor, VariableEditor *variableEditor);

private:
    QString path;
    QString  fileName;
    QString extension;
    DataEditor *dataEdit;

signals:

public slots:
};

#endif // TABEDITOR_H
