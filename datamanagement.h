#ifndef DATAMANAGEMENT_H
#define DATAMANAGEMENT_H

//#include <omp.h>
#include <RInside.h>
#include <QtGui>

struct type{
    double real;
    std::string string;
};

class DataManagement
{
public:
    DataManagement();
    ~DataManagement();
    void mergeSort(std::vector<std::vector<type> >& vec, std::vector<int> cols, std::vector<bool> order, QStringList types, int thread);
    void mergeSortInt(std::vector<std::vector<int> >& vec, std::vector<int> cols,std::vector<bool> order,int low, int high);
    QStringList getVarTypesFromDF(Rcpp::DataFrame DF);
    std::vector<std::vector<type> > V2ColtoRowType(std::vector<std::vector<type> > VM, QStringList varTypes);
    std::vector<std::vector<type> > V2RowtoColType(std::vector<std::vector<type> > VM, QStringList varTypes);

private:
    void mergeSortStruct(std::vector<std::vector<type> >& vec, std::vector<std::vector<type> >& temp,
                    int low, int high, std::vector<int> cols, std::vector<bool> order, QStringList types);
    void merge(std::vector<std::vector<type> >& vec, std::vector<std::vector<type> >& temp, int low,
                int mid, int high, std::vector<int> cols, std::vector<bool> order, QStringList types);
    void merge2V(std::vector<std::vector<type> >& vec1,std::vector<std::vector<type> >& vec2, std::vector<std::vector<type> >& temp,
                 int low1, int high1, std::vector<int> cols, std::vector<bool> order, QStringList types);
    void mergeInt(std::vector<std::vector<int> >& vec, std::vector<int> cols,std::vector<bool> order, int low, int mid, int high);

};

#endif // DATAMANAGEMENT_H
