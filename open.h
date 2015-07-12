#ifndef OPEN_H
#define OPEN_H

#include <QWidget>
#include <QtGui>
#include "rcore.h"
#include "activedataset.h"
#include <QTextStream>

class Open : public QWidget
{
    Q_OBJECT
public:
    explicit Open(QWidget *parent=0);
    ~Open();
    QString getPath();
    QString getFileName();
    QString getExtension();
    void setPath(QString path);
    void setFileName(QString fileName);
    void setExtension(QString extension);
    void setMainTab(QTabWidget* tab);
    void openFileDialog();
    void openEditor();
    bool checkParameter(QTextStream text, QChar ch);
    void openNewData(RCore* rCore,ActiveDataset* activeDatasetClass );


private:
    QString path;
    QString  fileName;
    QString extension;
    QTabWidget* mainTab;

signals:

public slots:
};

#endif // OPEN_H
