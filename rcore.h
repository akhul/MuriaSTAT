#ifndef RCORE_H
#define RCORE_H

#include <QWidget>
#include <QtGui>
#include <RInside.h>

class RCore
{
public:
    RCore(RInside &m_R);
    ~RCore();
    Rcpp::DataFrame getDataFrame(); //Update
    Rcpp::StringVector getColNames();
    QStringList getVarTypes(); //Update
    int getRow();
    int getCol();
    std::string getSyntaxOpen();
    void setDataFrame(Rcpp::DataFrame dataFrame); //Update
    void setColnames(Rcpp::StringVector colNames);
    void OpenData(std::string filePath, std::string fileName, std::string fileType);
    void diplayData(std::string name);
    void diplayData2(std::string name);
    void runSyntax(std::string syntax);
    void chooseType(std::string path,std::string dataType, std::string name);
    //--Update
    void setVarTypes(QStringList varTypes);
    void setDataName(std::string name);
    std::string getDataName();
    //Update 27 juni
    void setDataFrametoR(Rcpp::DataFrame DF, std::string name);

private:
    Rcpp::DataFrame dataFrame;
    Rcpp::StringVector colNamesData;
    RInside &R;
    std::string syntaxOpen;
    QStringList varTypes;
    std::string dataName;
    int row;
    int col;

    //method
    void setVarTypesFromDF(Rcpp::DataFrame DataFrame); //Update

signals:

public slots:
};

#endif // RCORE_H
