#include "rcore.h"

RCore::RCore(RInside &R): R(R)
{

}

RCore::~RCore()
{

}


Rcpp::DataFrame RCore::getDataFrame(){
    return dataFrame;
}
Rcpp::StringVector RCore::getColNames(){
    return colNamesData;
}
QStringList RCore::getVarTypes() //Update
{
    return varTypes;
}
int RCore::getCol(){
    return col;
}
int RCore::getRow(){
    return row;
}
std::string RCore::getSyntaxOpen(){
    return syntaxOpen;

}

void RCore::setColnames(Rcpp::StringVector colNames){
    colNamesData=colNames;
}
void RCore::setDataFrame(Rcpp::DataFrame data){
    dataFrame=data;
}


void RCore::OpenData(std::string path, std::string name, std::string type){

    chooseType(path,type,name);


    R.parseEvalQ(syntaxOpen);

   // Rcpp::NumericMatrix newDataEditor= R.parseEval(name+"<-as.matrix(tes)");
    Rcpp::StringVector newColNamesData =  R.parseEval("colnames("+name+")");
    setColnames(newColNamesData);
   // setDataEditor(newDataEditor);
    R.parseEvalQ("x<-dim("+name+")");
    row = R.parseEval("x[1]");
    col = R.parseEval("x[2]");

    if(col==1){
        R.parseEvalQ(name+ "<- read.csv('" +path+ "',sep=',',header=TRUE)");

       // Rcpp::NumericMatrix newDataEditor= R.parseEval(name+"<-as.matrix(tes)");
        Rcpp::StringVector newColNamesData =  R.parseEval("colnames("+name+")");
        setColnames(newColNamesData);
       // setDataEditor(newDataEditor);
        R.parseEvalQ("x<-dim("+name+")");
        row = R.parseEval("x[1]");
        col = R.parseEval("x[2]");


    }

}
void RCore::diplayData(std::string name){
    qDebug("Display data");
    Rcpp::DataFrame newDataFrame= R.parseEval(name+"<-as.data.frame("+name+")");
    Rcpp::StringVector newColNamesData =  R.parseEval("colnames("+name+")");
    setColnames(newColNamesData);
    setDataFrame(newDataFrame);
    R.parseEvalQ("x<-dim("+name+")");
    row = R.parseEval("x[1]");
    col = R.parseEval("x[2]");

    //--Update
    setVarTypesFromDF(newDataFrame);
    setDataName(name);
}
void RCore::diplayData2(std::string name){

    Rcpp::DataFrame newDataEditor= R.parseEval(name+"<-as.data.frame("+name+")");
    Rcpp::StringVector newColNamesData =  R.parseEval("colnames("+name+")");
    setColnames(newColNamesData);
    setDataFrame(newDataEditor);
    R.parseEvalQ("x<-dim("+name+")");
    row = R.parseEval("x[1]");
    col = R.parseEval("x[2]");

}

void RCore::runSyntax(std::string syntax){
    qDebug(QString::fromStdString(syntax).toLocal8Bit());
    R.parseEvalQ(syntax);
}

void RCore::chooseType(std::string path,std::string type, std::string name){
    if(type=="csv"){
    syntaxOpen=name+ "<- read.csv('" +path+ "',sep=';',header=TRUE)";
    }
    else if(type=="sav"){
    syntaxOpen="library(foreign);"+name+" <- read.spss('" +path+ "',to.data.frame=TRUE);";
    }
    else if(type=="xpt"){
    syntaxOpen="library(foreign);"+name+" <- read.xport('" +path+ "');";
    }
    else if(type=="dta"){
    syntaxOpen="library(foreign);"+name+" <- read.dta('" +path+ "')";
    }
    else{
        syntaxOpen=name+" <- read.delim('" +path+ "')";
    }
}

//---Update
void RCore::setVarTypes(QStringList varTypes1)
{
    varTypes = varTypes1;
}

void RCore::setDataName(std::string name)
{
    dataName = name;
}

std::string RCore::getDataName()
{
    return dataName;
}

void RCore::setVarTypesFromDF(Rcpp::DataFrame DF)
{
    qDebug("Rcore setvartypes");
    varTypes.clear();

    QRegExp re("[+-]?\\d*\\.?\\d+");
    for(int j=0; j < DF.size(); j++){
        begin:
        //qDebug("begin");
        qDebug() << j;
        Rcpp::CharacterVector vektor = DF[j];
        QString string =QString::fromUtf8(vektor[0]);

        if(string.contains("NA")){
            qDebug("Cotains NA");
            bool allNA = false;
            for(int i=1; i< vektor.size(); i++){
                string =QString::fromUtf8(vektor[i]);
                if(string.contains("NA")){
                    allNA = true;
                }
            }
            if(allNA){
                qDebug("All Cotains NA");
                varTypes.insert(j,"String");
                j++;
                if(j<DF.size()){
                    qDebug("goto begin");
                    goto begin;
                }else{
                    break;
                }
            }
        }

        if(!re.exactMatch(string) && !string.contains("NA")){
            //string
            //qDebug("String");
            varTypes.insert(j,"String");
        }else{
            //numeric

            bool validate;
            int lastIndex = 0;
            int result = 0;
            for (int i = 0; i < vektor.length(); i++) {
                if(!QString(vektor[i]).contains("NA") && !QString(vektor[i]).isEmpty()){
                    lastIndex = i;
                    QString string = QString::fromUtf8(vektor[i]);
                    //qDebug("------1");
                    //qDebug() << j;
                    //qDebug() << i;
                    //qDebug() << string.toLocal8Bit();
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
                    if(!QString(vektor[i]).contains("NA") && !QString(vektor[i]).isEmpty()){
                        QString string = QString::fromUtf8(vektor[i]);
                        //qDebug("------2");
                        //qDebug() << j;
                        //qDebug() << i;
                        //qDebug() << string.toLocal8Bit();
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
                    //qDebug("Integer");
                    varTypes.insert(j,"Integer");
                    break;
                case 2:
                    //qDebug("Real");
                    varTypes.insert(j,"Real");
                    break;
                case 3:

                    //qDebug("String");
                    varTypes.insert(j,"String");
                    break;
            }
        }
        //QMessageBox::information(0,"VARTYPE", varTypes.at(j));
    }
}
//-----

//Update 26 Juni
void RCore::setDataFrametoR(Rcpp::DataFrame DF, std::string name)
{
    qDebug("SetdataFrametoR");
    R[name] = DF;
}
