#include "tabeditor.h"

TabEditor::TabEditor(QWidget *parent) : QTabWidget(parent)
{

}

TabEditor::~TabEditor()
{

}
DataEditor *TabEditor::getDataEdit(){
    return dataEdit;
}

void TabEditor::setExtension(QString ext){
    extension = ext;
}
void TabEditor::setFileName(QString fileNames){
    fileName=fileNames;
}
void TabEditor::setPath(QString filePath){
    path = filePath;
}

QString TabEditor::getExtension(){
    return extension;
}
QString TabEditor::getFileName(){
    return fileName;
}

QString TabEditor::getPath(){
    return path;
}

void TabEditor::openFileDialog(){

    QString file = QFileDialog::getOpenFileName(this,
                                              "Select source file",
                                              ".",
                                              "Text files (*.csv)"
                                              ";;csv files (*.txt)"
                                              ";;csv files (*.sav)"
                                              ";;csv files (*.xpt)"
                                              ";;csv files (*.dpt)"
                                              ";;All files (*.*)");

   file.replace("/","//");
   setFileName(QFileInfo(file).baseName());
   setExtension(QFileInfo(file).suffix());
   setPath(file);

}

void TabEditor::displayEditor(RCore *rExe,std::string name, DataEditor *dataEditor, VariableEditor *variableEditor){
    qDebug("Display Editor");
    rExe->diplayData(name);
    //----Update
    dataEditor->setVarTypes(rExe->getVarTypes());
    QStringList varNames;
    Rcpp::StringVector sv = rExe->getColNames();
    for(int i = 0; i < sv.size();i++){
        varNames.push_back(QString(sv[i]));
    }
    dataEditor->setVarNames(varNames);
    dataEditor->loadData(rExe->getDataFrame(),rExe->getColNames());
    //---
    variableEditor->loadVariable(rExe,rExe->getColNames());

    addTab(dataEditor,"Data");
    addTab(variableEditor, "Variable");
    setTabPosition(West);
    //Update 30 Juni
    connect(this, SIGNAL(currentChanged(int)), variableEditor, SLOT(checkWidgetVisibility()));
}

//Update 26 Juni
void TabEditor::newEditor(DataEditor *dataEditor, VariableEditor *variableEditor)
{
    addTab(dataEditor,"Data");
    addTab(variableEditor, "Variable");
    setTabPosition(West);
    connect(this, SIGNAL(currentChanged(int)), variableEditor, SLOT(checkWidgetVisibility()));
}
