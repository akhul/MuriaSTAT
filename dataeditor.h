#ifndef DATAEDITOR_H
#define DATAEDITOR_H

#include <QTableWidget>
#include <RInside.h>
#include "datamanagement.h"

class DataEditor : public QTableWidget
{
    Q_OBJECT
public:
    explicit DataEditor(QWidget *parent = 0);
    ~DataEditor();
    void clear();
    void setVarTypes(QStringList type);
    void setVarNames(QStringList name);
    QTableWidgetSelectionRange selectedRange() const;
    void loadData(Rcpp::DataFrame dataFrame, Rcpp::StringVector colNames);
    //Update 27 Juni
    std::vector<std::vector<int> > getIndexChanged();
    void clearIdxChanged();
    void setScrollConnect();
    //Update 30 Juni
    void setVM(std::vector<std::vector<type> > VM1);
    void reloadTable(bool colNames, bool sorted);
    void loadVarReal(std::vector<type> V, int col, QString varName, QString varType);
    void loadVarStr(std::vector<type> V, int col, QString varName, QString varType);

    std::vector<std::vector<type> > getVM();
    void setVMsorted(std::vector<std::vector<type> > VM1);
    QStringList getVarTypes();
    Rcpp::DataFrame getDataFrame();
    bool isReload;
    bool isSorted;
    void changeVarType(int col, QString type);
    void deleteVar(int col);
    void insertNewVar(int col);

private:
    QString formula(int row, int column) const;
    QStringList varTypes;
    QStringList varNames;
    int idxScroll;
    std::vector<std::vector<type> > VM;
    //update 27 juni
    std::vector<std::vector<int> > indexChanged;
    std::vector<std::string> oldCellText;
    void keyPressEvent(QKeyEvent* event);

    std::vector<std::vector<type> > VMsorted;
    Rcpp::DataFrame dataFrame;
    void addIndexChanged(int row, int column);

    void deleteRow(int row);
    void insertNewRow(int row);

private slots:
    void somethingChanged();
    //--Update 27 Juni
    void scrollMoved(int i);
    void cellValidation(int row, int column);

signals:
    void modified();

public slots:
    void addrow();
    void del();
    void cut();
    void copy();
    void paste();
    //Update 8 Juli
    void showRowContextMenu(const QPoint&);
    void showColContextMenu(const QPoint&);
};

#endif // DATAEDITOR_H
