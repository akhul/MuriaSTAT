#include "datamanagement.h"

DataManagement::DataManagement()
{

}

DataManagement::~DataManagement()
{

}

QStringList DataManagement::getVarTypesFromDF(Rcpp::DataFrame DF)
{
    QStringList varTypes;
    QRegExp re("[+-]?\\d*\\.?\\d+");
    for(int j=0; j < DF.size(); j++){

        Rcpp::CharacterVector vektor = DF[j];
        QString string =QString::fromUtf8(vektor[0]);

        if(!re.exactMatch(string) && !string.contains("NA")){
            //string
            varTypes.insert(j,"String");
        }else{
            //numeric

            bool validate;
            int lastIndex = 0;
            int result = 0;
            for (int i = 0; i < vektor.length(); i++) {
                if(!QString(vektor[i]).contains("NA")){
                    lastIndex = i;
                    QString string = QString::fromUtf8(vektor[i]);
                    string.toInt(&validate);

                    if(validate == false){
                        if (!re.exactMatch(string)){
                            result = 3;
                        }
                        lastIndex = i;
                        break;
                    }
                    result = 1;
                }else{
                    lastIndex++;
                }
            }
            if (lastIndex+1 != vektor.length()) {
                for (int i = lastIndex; i <  vektor.length(); i++) {
                    if(!QString(vektor[i]).contains("NA")){
                        QString string = QString::fromUtf8(vektor[i]);
                        string.toDouble(&validate);
                        if(validate == false){

                            if (!re.exactMatch(string)){
                                result = 3;
                            }
                            break;
                        }
                        result = 2;
                    }
                }
            }
            switch (result) {
                case 1:
                    varTypes.insert(j,"Integer");
                    break;
                case 2:
                    varTypes.insert(j,"Real");
                    break;
                case 3:
                    varTypes.insert(j,"String");
                    break;
            }
        }
        //QMessageBox::information(0,"VARTYPE", varTypes.at(j));
    }
    return varTypes;
}

std::vector<std::vector<type> > DataManagement::V2RowtoColType(std::vector<std::vector<type> > VM, QStringList varTypes)
{
    std::vector<std::vector<type> > VM1(VM[0].size(), std::vector<type>(VM.size()));
    for(unsigned i =0; i< VM.size();i++ ){
        for(unsigned j =0; j< VM[0].size();j++ ){
            if(varTypes.at(j)=="String"){
                VM1[j][i].string = VM[i][j].string;
            }else{
                VM1[j][i].real = VM[i][j].real;
            }
        }
    }
    return VM1;
}

std::vector<std::vector<type> > DataManagement::V2ColtoRowType(std::vector<std::vector<type> > VM, QStringList varTypes)
{
    std::vector<std::vector<type> > VM1(VM[0].size(), std::vector<type>(VM.size()));
    for(unsigned i =0; i< VM[0].size();i++ ){
        for(unsigned j =0; j< VM.size();j++ ){
            if(varTypes.at(j)=="String"){
                VM1[i][j].string = VM[j][i].string;
            }else{
                VM1[i][j].real = VM[j][i].real;
            }
        }
    }
    return VM1;
}


void DataManagement::mergeSort(std::vector<std::vector<type> > &VM1, std::vector<int> cols, std::vector<bool> order, QStringList types, int thread)
{
    int colCount = VM1[0].size();
    std::vector<std::vector<type> > temp1(VM1.size(), std::vector<type>(colCount));
    if(VM1.size() < 100){
        goto serial;
    }
    if(thread == 1){ //Serial
        serial:
        //double t = omp_get_wtime();
        mergeSortStruct(VM1,temp1,0,VM1.size()-1,cols,order, types);
        //qDebug("Serial");
        //qDebug() << omp_get_wtime() -t;
    }else if(thread <4){ //Par2
        /*int low1, high1, low2, high2;
        int mod = VM1.size()%2;
        int size = VM1.size()/2;
        low1 = 0;
        high1 = mod+size-1;
        low2 = high1+1;
        high2 = low2+size-1;

        std::vector<std::vector<type> > VM2b(VM1.size()/2, std::vector<type>(colCount));
        std::vector<std::vector<type> > temp2b(VM1.size()/2, std::vector<type>(colCount));
        for(int i=low2;i<=high2;i++)
        {
            VM2b[i-low2] = VM1[i];
        }

        double t = omp_get_wtime();
        #pragma omp parallel sections
        {
            #pragma omp section
            {
                mergeSortStruct(VM1,temp1,low1,high1,cols,order, types);
            }
            #pragma omp section
            {
                mergeSortStruct(VM2b,temp2b,0,size-1,cols,order, types);
            }
        }
        merge2V(VM1,VM2b,temp1,low1,high1,cols,order, types);

        qDebug("Par2");
        qDebug() << omp_get_wtime() -t;*/
    }else{ // Par4
        /*int low1, high1, low2, high2, low3, high3, low4, high4;
        int mod = VM1.size()%4;
        int size = VM1.size()/4;
        low1 = 0;
        high1 = mod+size-1;
        low2 = high1+1;
        high2 = low2+size-1;
        low3 = high2+1;
        high3 = low3+size-1;
        low4 = high3+1;
        high4 = low4+size-1;

        std::vector<std::vector<type> > VM4b(VM1.size()/4, std::vector<type>(colCount));
        std::vector<std::vector<type> > VM4c(VM1.size()/2, std::vector<type>(colCount));
        std::vector<std::vector<type> > VM4d(VM1.size()/4, std::vector<type>(colCount));
        std::vector<std::vector<type> > temp4b(VM1.size()/4, std::vector<type>(colCount));
        std::vector<std::vector<type> > temp4c(VM1.size()/2, std::vector<type>(colCount));
        std::vector<std::vector<type> > temp4d(VM1.size()/4, std::vector<type>(colCount));
        for(int i=low2; i<=high4; i++){
            if( i <= high2){
                VM4b[i-low2] = VM1[i];
            }else if(i <= high3){
                VM4c[i-low3] = VM1[i];
            }else{
                VM4d[i-low4] = VM1[i];
            }
        }
        double t = omp_get_wtime();
        #pragma omp parallel sections
        {
            #pragma omp section
            {
                mergeSortStruct(VM1,temp1,low1,high1,cols,order, types);
            }
            #pragma omp section
            {
                mergeSortStruct(VM4b,temp4b,0,size-1,cols,order, types);
            }
            #pragma omp section
            {
                mergeSortStruct(VM4c,temp4c,0,size-1,cols,order, types);
            }
            #pragma omp section
            {
                mergeSortStruct(VM4d,temp4d,0,size-1,cols,order, types);
            }
        }
        #pragma omp parallel sections
        {
            #pragma omp section
            {
                merge2V(VM1,VM4b,temp1,low1,high1,cols,order, types);
            }
            #pragma omp section
            {
                int mid = (low3+high4)/2-low3;
                merge2V(VM4c,VM4d,temp4c,0,mid,cols,order, types);
            }
        }
        merge2V(VM1,VM4c,temp1,low1,high2,cols,order, types);
        qDebug("Par4");
        qDebug() << omp_get_wtime() -t;*/
    }
}

void DataManagement::mergeSortStruct(std::vector<std::vector<type> >& vec,std::vector<std::vector<type> >& temp,
                               int low, int high, std::vector<int> cols, std::vector<bool> order, QStringList types)
{
    int mid;
    if(low < high){
        mid = (low+high)/2;
        mergeSortStruct(vec,temp,low, mid,cols, order, types);
        mergeSortStruct(vec,temp,mid+1, high,cols, order, types);
        merge(vec,temp,low, mid, high,cols, order, types);
    }
}

void DataManagement::merge(std::vector<std::vector<type> >& vec,std::vector<std::vector<type> >& temp,
                           int low, int mid, int high, std::vector<int> cols,std::vector<bool> order, QStringList types)
{
    int i,j,k;
    i=low;
    k=0;
    j=mid+1;

    while(i <= mid && j <= high)
    {
        for(unsigned col = 0; col < cols.size(); col++){
            unsigned c = cols[col];
            bool bool1, bool2;
            if(types[col] == "String"){
                bool1 = vec[i][c].string < vec[j][c].string;
                bool2 = vec[i][c].string > vec[j][c].string;
            }else{
                bool1 = vec[i][c].real < vec[j][c].real;
                bool2 = vec[i][c].real > vec[j][c].real;
            }
            if(bool1)
            {
                if(order[col]){
                    temp[k] = vec[i];
                    k++;
                    i++;
                }
                else{
                    temp[k] = vec[j];
                    k++;
                    j++;
                }
                break;
            }
            else if(bool2)
            {
                if(order[col]){
                    temp[k] = vec[j];
                    k++;
                    j++;
                }
                else{
                    temp[k] = vec[i];
                    k++;
                    i++;
                }
                break;
            }
            else
            {
                if(cols.size()==1)
                {
                    temp[k] = vec[i];
                    k++;
                    i++;
                    break;
                }
                else{
                    c = cols[col+1];
                    if(types[col+1] == "String"){
                        bool1 = vec[i][c].string < vec[j][c].string;
                        bool2 = vec[i][c].string > vec[j][c].string;
                    }else{
                        bool1 = vec[i][c].real < vec[j][c].real;
                        bool2 = vec[i][c].real > vec[j][c].real;
                    }

                    if(bool1)
                    {
                        if(order[col+1]){
                            temp[k] = vec[i];
                            k++;
                            i++;
                        }
                        else{
                            temp[k] = vec[j];
                            k++;
                            j++;
                        }
                        break;
                    }
                    else if(bool2)
                    {
                        if(order[col+1]){
                            temp[k] = vec[j];
                            k++;
                            j++;
                        }
                        else{
                            temp[k] = vec[i];
                            k++;
                            i++;
                        }
                        break;
                    }
                    else
                    {
                        if(col==(cols.size()-2)){
                            temp[k] = vec[i];
                            k++;
                            i++;
                            break;
                        }
                    }
                }
            }
        }
    }

    // Push the remaining data from both vectors onto the resultant
    while(i <= mid)
    {
        temp[k] = vec[i];
        k++;
        i++;
    }

    while(j <= high)
    {
        temp[k] = vec[j];
        k++;
        j++;
    }
    for (i = 0; i < k; i++)
    {
        vec[low++] = temp[i];
    }
}

void DataManagement::merge2V(std::vector<std::vector<type> >& vec1,std::vector<std::vector<type> >& vec2,std::vector<std::vector<type> >& temp,
                            int low1, int high1, std::vector<int> cols, std::vector<bool> order, QStringList types)
{

    int i,j,k, high2;
    i=low1;
    k=0;
    j=0;
    high2 = vec2.size()-1;

    while(i <= high1 && j <= high2)
    {
        for(unsigned col = 0; col < cols.size(); col++){
            unsigned c = cols[col];
            bool bool1, bool2;
            if(types[col] == "String"){
                bool1 = vec1[i][c].string < vec2[j][c].string;
                bool2 = vec1[i][c].string > vec2[j][c].string;
            }else{
                bool1 = vec1[i][c].real < vec2[j][c].real;
                bool2 = vec1[i][c].real > vec2[j][c].real;
            }
            if(bool1)
            {
                if(order[col]){
                    temp[k] = vec1[i];
                    k++;
                    i++;
                }
                else{
                    temp[k] = vec2[j];
                    k++;
                    j++;
                }
                break;
            }
            else if(bool2)
            {
                if(order[col]){
                    temp[k] = vec2[j];
                    k++;
                    j++;
                }
                else{
                    temp[k] = vec1[i];
                    k++;
                    i++;
                }
                break;
            }
            else
            {
                if(cols.size()==1)
                {
                    temp[k] = vec1[i];
                    k++;
                    i++;
                    break;
                }
                else{
                    c = cols[col+1];
                    if(types[col+1] == "String"){
                        bool1 = vec1[i][c].string < vec2[j][c].string;
                        bool2 = vec1[i][c].string > vec2[j][c].string;
                    }else{
                        bool1 = vec1[i][c].real < vec2[j][c].real;
                        bool2 = vec1[i][c].real > vec2[j][c].real;
                    }

                    if(bool1)
                    {
                        if(order[col+1]){
                            temp[k] = vec1[i];
                            k++;
                            i++;
                        }
                        else{
                            temp[k] = vec2[j];
                            k++;
                            j++;
                        }
                        break;
                    }
                    else if(bool2)
                    {
                        if(order[col+1]){
                            temp[k] = vec2[j];
                            k++;
                            j++;
                        }
                        else{
                            temp[k] = vec1[i];
                            k++;
                            i++;
                        }
                        break;
                    }
                    else
                    {
                        if(col==(cols.size()-2)){
                            temp[k] = vec1[i];
                            k++;
                            i++;
                            break;
                        }
                    }
                }
            }
        }
    }

    // Push the remaining data from both vectors onto the resultant
    while(i <= high1)
    {
        temp[k] = vec1[i];
        k++;
        i++;
    }

    while(j <= high2)
    {
        temp[k] = vec2[j];
        k++;
        j++;
    }
    for (i = 0; i < k; i++)
    {
        vec1[low1++] = temp[i];
    }
}


void DataManagement::mergeSortInt(std::vector<std::vector<int> >& vec, std::vector<int> cols,std::vector<bool> order,int low, int high)
{
    int mid;
    if(low < high){
        mid = (low+high)/2;
        mergeSortInt(vec,cols,order,low, mid);
        mergeSortInt(vec,cols,order,mid+1, high);
        mergeInt(vec,cols,order,low, mid, high);
    }
}

void DataManagement::mergeInt(std::vector<std::vector<int> >& vec, std::vector<int> cols,std::vector<bool> order, int low, int mid, int high)
{
    std::vector<std::vector<int> > temp;
    int i,j;
    i=low;
    j=mid+1;

    while(i <= mid && j <= high)
    {
        for(unsigned k = 0; k < cols.size(); k++){
            unsigned c = cols[k];
            if(vec[i][c] < vec[j][c])
            {
                if(order[k]){
                    temp.push_back(vec[i]);
                    i++;
                }
                else{
                    temp.push_back(vec[j]);
                    j++;
                }
                break;
            }
            else if(vec[i][c] > vec[j][c])
            {
                if(order[k]){
                    temp.push_back(vec[j]);
                    j++;
                }
                else{
                    temp.push_back(vec[i]);
                    i++;
                }
                break;
            }
            else
            {
                if(cols.size()==1)
                {
                    temp.push_back(vec[i]);
                    i++;
                    break;
                }
                else{
                    if(vec[i][c+1] < vec[j][c+1])
                    {
                        if(order[k+1]){
                            temp.push_back(vec[i]);
                            i++;
                        }
                        else{
                            temp.push_back(vec[j]);
                            j++;
                        }
                        break;
                    }
                    else if(vec[i][c+1] > vec[j][c+1])
                    {
                        if(order[k+1]){
                            temp.push_back(vec[j]);
                            j++;
                        }
                        else{
                            temp.push_back(vec[i]);
                            i++;
                        }
                        break;
                    }
                    else
                    {
                        if(k==(cols.size()-2)){
                            temp.push_back(vec[i]);
                            i++;
                            break;
                        }
                    }
                }
            }
        }
    }

    // Push the remaining data from both vectors onto the resultant
    while(i <= mid)
    {
        temp.push_back(vec[i]);
        i++;
    }

    while(j <= high)
    {
        temp.push_back(vec[j]);
        j++;
    }
    for (i = 0;(unsigned) i <  temp.size(); i++)
    {
        vec[low++] = temp[i];
    }
}
