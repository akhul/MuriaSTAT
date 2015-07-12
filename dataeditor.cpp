#include <QtGui>
#include "dataeditor.h"
#include "activedataset.h"
#include "mainwindow.h"

DataEditor::DataEditor(QWidget *parent) : QTableWidget(parent)
{
    verticalHeader()->setDefaultSectionSize(24);
    horizontalHeader()->setDefaultSectionSize(80);
    verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
    verticalHeader()->setResizeMode(QHeaderView::Fixed);
    //connect(this,SIGNAL(itemSelectionChanged()),this,SLOT(addrow()));
    //connect(this, SIGNAL(itemChanged(QTableWidgetItem *)),this, SLOT(somethingChanged()));
    clear();
    //update 27 Juni
    setRowCount(50);
    isSorted = false;
    isReload = false;
    connect(this, SIGNAL(cellChanged(int,int)),this, SLOT(cellValidation(int,int)));
    //Update 8 Juli
    verticalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(verticalHeader(),SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(showRowContextMenu(QPoint)));
    connect(horizontalHeader(),SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(showColContextMenu(QPoint)));
}

DataEditor::~DataEditor()
{

}

void DataEditor::clear()
{
    //reset data editor
    blockSignals(true);
    setRowCount(0);
    setColumnCount(0);
    setRowCount(50);
    setColumnCount(50);

    for (int i = 0; i <50; ++i) {
        QTableWidgetItem *item = new QTableWidgetItem;

            item->setText("var"+QString::number(i+1));
            setHorizontalHeaderItem(i, item);

    }

    setCurrentCell(0,0);
    blockSignals(false);
}

void DataEditor::addrow(){
   if(this->currentRow()==this->rowCount()-10){
    insertRow(rowCount());



   }
   if(this->currentColumn()==this->columnCount()-10){
    insertColumn(columnCount());
    QTableWidgetItem *item = new QTableWidgetItem;

    item->setText("var"+QString::number(columnCount()));
    setHorizontalHeaderItem(columnCount()-1, item);

   }


}

QString DataEditor::formula(int row, int column) const
{
    if(this->item(row, column)){
        return this->item(row, column)->text();
    }else{
        return "";
    }
}

void DataEditor::cut()
{
    qDebug("cut");
    copy();
    del();
}

void DataEditor::del()
{
    QTableWidgetSelectionRange range = selectedRange();
    for (int i = range.topRow(); i <= range.bottomRow(); ++i) {
        for (int j = range.leftColumn(); j <= range.rightColumn(); ++j) {
            QTableWidgetItem *item = new QTableWidgetItem;
            item->setText("");
            setItem(i, j, item);

            //update 30 Juni
            addIndexChanged(i,j);
        }
    }
}

void DataEditor::copy()
{
    qDebug("copy");
    QTableWidgetSelectionRange range = selectedRange();
    QString str;

    for (int i = 0; i < range.rowCount(); ++i) {
        if (i > 0)
            str += "\n";
        for (int j = 0; j < range.columnCount(); ++j) {
            if (j > 0)
                str += "\t";
            str += formula(range.topRow() + i, range.leftColumn() + j);
        }
    }
    QApplication::clipboard()->setText(str);
}

void DataEditor::paste()
{
    QTableWidgetSelectionRange range = selectedRange();
    QString str = QApplication::clipboard()->text();
    if(str.endsWith('\n')){
      str=str.left(str.length()-1);
    }
    QStringList rows = str.split('\n');
    int numRows = rows.count();
    int numColumns = rows.first().count('\t') + 1;
    if (range.rowCount() * range.columnCount() != 1
            && (range.rowCount() != numRows
                || range.columnCount() != numColumns)) {
        QMessageBox::information(this, tr("Spreadsheet"),
                tr("The information cannot be pasted because the copy "
                   "and paste areas aren't the same size."));
        return;
    }
    for (int i = 0; i < numRows; ++i) {
        QStringList columns = rows[i].split('\t');
        for (int j = 0; j < numColumns; ++j) {
            int row = range.topRow() + i;
            int column = range.leftColumn() + j;
            if (row < this->rowCount() && column < this->columnCount()){
                QTableWidgetItem *item = new QTableWidgetItem;
                item->setText(columns[j]);
                setItem(row, column, item);
        }
    }
}}

QTableWidgetSelectionRange DataEditor::selectedRange() const
{
    QList<QTableWidgetSelectionRange> ranges = selectedRanges();
    if (ranges.isEmpty())
        return QTableWidgetSelectionRange();
    return ranges.first();
}

void DataEditor::somethingChanged()
{
    emit modified();
    //rowChanged
}

//------------Update
void DataEditor::setVarTypes(QStringList type){
    varTypes=type;
}

void DataEditor::changeVarType(int col, QString newType)
{
    blockSignals(true);
    if(varTypes.at(col) != newType){
        qDebug("Validasi tipe ganti");
        QRegExp re("[+-]?\\d*\\.?\\d+");
        for(unsigned row=0; row < VM.size(); row++){
            if(item(row, col)->text() != "" && col < varTypes.size()){
                QString string = item(row,col)->text();
                QString type;

                if(!re.exactMatch(string)){
                    //string
                    type ="String";
                }else{
                    //numeric
                    bool validate;
                    string.toInt(&validate);
                    if(validate == true){
                        type ="Integer";
                        goto check_1;
                    }
                    string.toDouble(&validate);
                    if(validate == true){
                        type ="Real";
                    }
                }
                check_1:
                //qDebug(type.toLocal8Bit());
                if(newType == type || (newType == "Real" && type == "Integer")){
                    qDebug("Sama");
                }else {
                    qDebug("Beda");
                    if(item(row,col)->text() == "NA"){
                        item(row,col)->setText("NA");
                    }else{
                        item(row,col)->setText("");
                    }
                }

                if(newType == "String"){
                    if(item(row,col)->text() == ""){
                        VM[row][col].string = "NA";
                    }else{
                        VM[row][col].string = item(row,col)->text().toStdString();

                    }
                }else{
                    if(item(row,col)->text() == "NA"){
                        VM[row][col].real = std::numeric_limits<double>::quiet_NaN();
                    }
                    else{
                        VM[row][col].real = item(row,col)->text().toDouble();
                    }
                }
            }
        }

        varTypes.replace(col,newType);
    }
    blockSignals(false);
}

void DataEditor::setVarNames(QStringList name){
    varNames=name;
}

Rcpp::DataFrame DataEditor::getDataFrame()
{
    qDebug("get dataFrame");
    //Jika ada proses data management atau perubahan sebelumnya, maka nilai vector berubah. Oleh karena itu dataframe didapatkan dari vector yg terbaru
    //jika tidak, maka yg dikirim langsung adalah dataframe yg sudah ada
    qDebug()<< indexChanged.size();
    qDebug() << VM.size();
    qDebug() << VM[0].size();
    qDebug() << varTypes.size();
    qDebug() << varNames.size();

    if(isReload || indexChanged.size() > 0){
        //SetVM to DF
        qDebug("is reload or changed");

        if(dataFrame.size() > 0){
            Rcpp::CharacterVector charVector = dataFrame[0];
            if(VM.size() != (unsigned) charVector.size()){
                goto newDF;
            }

            if(VM[0].size() > (unsigned) dataFrame.size()){
                //-------------------------------------------Buat kolom baru
                qDebug("VM > DF col");
                for(unsigned j =0; j< VM[0].size();j++ ){
                    if(varTypes.at(j)=="String"){
                        std::vector<std::string> V;
                        for(unsigned i=0; i< VM.size(); i++){
                            int index = i;
                            if(isSorted && VMsorted.size()>0){
                                index = VMsorted[i][0].real;
                            }
                            V.push_back(VM[index][j].string);
                        }
                        if(j < (unsigned) dataFrame.size()){
                            dataFrame[j] = V;
                        }else{
                            dataFrame.push_back(V);
                        }
                    }
                    else if(varTypes.at(j)=="Integer"){
                        std::vector<int> Vi;
                        for(unsigned i=0; i< VM.size(); i++){
                            int index = i;
                            if(isSorted && VMsorted.size()>0){
                                index = VMsorted[i][0].real;
                            }

                            if(VM[index][j].real == VM[index][j].real){
                                Vi.push_back(VM[index][j].real);
                            }else{
                                Vi.push_back(std::numeric_limits<double>::quiet_NaN());
                            }

                        }
                        if(j < (unsigned) dataFrame.size()){
                            dataFrame[j] = Vi;
                        }else{
                            dataFrame.push_back(Vi);
                        }
                    }else{
                        std::vector<double> Vd;
                        for(unsigned i=0; i< VM.size(); i++){
                            int index = i;
                            if(isSorted && VMsorted.size()>0){
                                index = VMsorted[i][0].real;
                            }
                            if(VM[index][j].real == VM[index][j].real){
                                Vd.push_back(VM[index][j].real);
                            }else{
                                Vd.push_back(std::numeric_limits<double>::quiet_NaN());
                            }
                        }
                        if(j < (unsigned) dataFrame.size()){
                            dataFrame[j] = Vd;
                        }else{
                            dataFrame.push_back(Vd);
                        }
                    }
                }
                qDebug("After add DF");
                Rcpp::StringVector colNames;
                for(int i=0; i<varNames.size(); i++){
                    colNames.push_back(varNames[i].toStdString());
                }
                dataFrame.attr("names") = colNames;
                Rcpp::Language call("as.data.frame",dataFrame);
                dataFrame = call.eval();
            }else{
                if(VM[0].size() == (unsigned) dataFrame.size()){
                    qDebug("VM = DF col");
                }else{
                    qDebug("VM < DF col");
                    //------------------------------------hapus kolom DF yg lebih
                    dataFrame.erase(VM[0].size(), dataFrame.size());
                }
                for(unsigned j =0; j< VM[0].size();j++ ){
                    if(varTypes.at(j)=="String"){
                        std::vector<std::string> V;
                        for(unsigned i=0; i< VM.size(); i++){
                            int index = i;
                            if(isSorted && VMsorted.size()>0){
                                index = VMsorted[i][0].real;
                            }
                            V.push_back(VM[index][j].string);
                        }
                        dataFrame[j] = V;
                    }
                    else if(varTypes.at(j)=="Integer"){
                        std::vector<int> Vi;
                        for(unsigned i=0; i< VM.size(); i++){
                            int index = i;
                            if(isSorted && VMsorted.size()>0){
                                index = VMsorted[i][0].real;
                            }
                            Vi.push_back(VM[index][j].real);
                        }
                        dataFrame[j] = Vi;
                    }else{
                        std::vector<double> Vd;
                        for(unsigned i=0; i< VM.size(); i++){
                            int index = i;
                            if(isSorted && VMsorted.size()>0){
                                index = VMsorted[i][0].real;
                            }
                            Vd.push_back(VM[index][j].real);
                        }
                        dataFrame[j] = Vd;
                    }
                }
            }
        }else{
            //---------------------------------Buat DF baru
            newDF:
            qDebug("New DF");
            dataFrame.erase(0, dataFrame.size());
            for(unsigned j =0; j< VM[0].size();j++ ){
                if(varTypes.at(j)=="String"){
                    std::vector<std::string> V;
                    for(unsigned i=0; i< VM.size(); i++){
                        int index = i;
                        if(isSorted && VMsorted.size()>0){
                            index = VMsorted[i][0].real;
                        }
                        V.push_back(VM[index][j].string);
                    }
                    dataFrame.push_back(V);
                }
                else if(varTypes.at(j)=="Integer"){
                    std::vector<int> Vi;
                    for(unsigned i=0; i< VM.size(); i++){
                        int index = i;
                        if(isSorted && VMsorted.size()>0){
                            index = VMsorted[i][0].real;
                        }
                        if(VM[index][j].real == VM[index][j].real){
                            Vi.push_back(VM[index][j].real);
                        }else{
                            Vi.push_back(std::numeric_limits<double>::quiet_NaN());
                        }
                    }
                    dataFrame.push_back(Vi);
                }else{
                    std::vector<double> Vd;
                    for(unsigned i=0; i< VM.size(); i++){
                        int index = i;
                        if(isSorted && VMsorted.size()>0){
                            index = VMsorted[i][0].real;
                        }
                        if(VM[index][j].real == VM[index][j].real){
                            Vd.push_back(VM[index][j].real);
                        }else{
                            Vd.push_back(std::numeric_limits<double>::quiet_NaN());
                        }
                    }
                    dataFrame.push_back(Vd);
                }
            }
            qDebug("After add DF");
            Rcpp::StringVector colNames;
            for(int i=0; i<varNames.size(); i++){
                colNames.push_back(varNames[i].toStdString());
            }
            dataFrame.attr("names") = colNames;
            Rcpp::Language call("as.data.frame",dataFrame);
            dataFrame = call.eval();
        }
        isReload = false;
    }
    return dataFrame;
}

void DataEditor::loadData(Rcpp::DataFrame dataFrame1, Rcpp::StringVector colNames)
{
    //Load data dari active dataset yg didisplay
    clear(); //Clear juga terdapat block signal false dan true, jadi harus ditaruh sebelumnya blockSignal true
    blockSignals(true);
    isReload = false;
    dataFrame = dataFrame1;
    for (int i = 0; i < colNames.size(); ++i) {
        QTableWidgetItem *item = new QTableWidgetItem;
        item->setText(QString(colNames[i]));
        setHorizontalHeaderItem(i, item);
    }
    Rcpp::CharacterVector vektor = dataFrame[0];
    VM.resize(vektor.size(), std::vector<type>(dataFrame.size()));
    for (int j=0; j<dataFrame.size(); j++) {
        int row = vektor.size();
        if(row > 10000){
            row = 50;
            setRowCount(row);
        }else{
            setRowCount(row+20);
        }
        vektor = dataFrame[j];
        for (int i=0; i<vektor.size(); i++) { //untuk load ke tabel dan set Vector (type)
            QString string;
            string =QString::fromUtf8(vektor[i]);
            if(varTypes.at(j)=="String"){
                if(string == ""){
                    VM[i][j].string = "NA";
                }else{
                    VM[i][j].string = string.toStdString();
                }
            }else{
                if(string == "NA"){
                    VM[i][j].real = std::numeric_limits<double>::quiet_NaN();
                }
                else{
                    VM[i][j].real = string.toDouble();
                }
            }
            if(i < row){ //Untuk load ke tabel
                QTableWidgetItem *item = new QTableWidgetItem;
                if(varTypes.at(j)=="String"){
                    item->setText(QString::fromStdString(VM[i][j].string));
                }else{
                    if(VM[i][j].real == VM[i][j].real){
                        item->setText(QString::number(VM[i][j].real));
                    }else{
                        item->setText("NA");
                    }
                }
                //qDebug() << string.toLocal8Bit();
                item->setTextAlignment(Qt::AlignCenter);
                setItem(i,j,item);
            }
        }
    }
    blockSignals(false);
    QScrollBar* pScroll= verticalScrollBar();
    if(pScroll){
        connect(pScroll,SIGNAL(valueChanged(int)),this,SLOT(scrollMoved(int)));
    }
    idxScroll = 0;
    //update 27 Juni
    //connect(this, SIGNAL(cellChanged(int,int)),this, SLOT(addIndexChanged(int,int)));
    //connect(this, SIGNAL(cellChanged(int,int)),this, SLOT(cellValidation(int,int)));
}

void DataEditor::scrollMoved(int i)
{
    //Saat scroll, jika jumlah baris vector > baris data editor, maka buat baris baru dan load data pada baris tsb
    blockSignals(true);
    qDebug("Scroll");
    //qDebug() << i;
    //qDebug() << idxScroll;
    //qDebug() << VM.size();
    if(i > idxScroll){
        int range = rowCount() + i-idxScroll;
        if(VM.size() > 10000 && (unsigned)rowCount()<=VM.size()){
            int idx = rowCount();
            setRowCount(range);
            for(int i=idx; i<range; i++){
                for(unsigned j=0;j<VM[i].size();j++)
                {
                    int index = i;
                    if(isSorted && VMsorted.size()>0){
                        index = VMsorted[i][0].real;
                    }
                    QTableWidgetItem *item = new QTableWidgetItem;
                    if(varTypes[j] == "String"){
                        item->setText(QString::fromStdString(VM[index][j].string));
                    }else{
                        if(VM[i][j].real == VM[i][j].real){
                            item->setText(QString::number(VM[index][j].real));
                        }else{
                            item->setText("NA");
                        }
                    }
                    item->setTextAlignment(Qt::AlignCenter);
                    setItem(i,j, item);
                    //qDebug("set");
                    //qDebug() << i;
                    //qDebug(item->text().toLocal8Bit());
                }
            }
        }else{
            setRowCount(range);
        }
        idxScroll = i;
    }
    blockSignals(false);
}

//---Update 26 Juni
void DataEditor::setScrollConnect()
{
    QScrollBar* pScroll= verticalScrollBar();
    if(pScroll){
        connect(pScroll,SIGNAL(valueChanged(int)),this,SLOT(scrollMoved(int)));
    }
    idxScroll = 0;
}

void DataEditor::addIndexChanged(int row, int column)
{
    blockSignals(true);
    //Menambahkan index baris dan kolom jika ada perubahan di data editor
    qDebug(">>>>> add Index Changed");
    item(row,column)->setTextAlignment(Qt::AlignCenter);
    qDebug() << indexChanged.size();
    std::vector<int> V;
    V.push_back(row);
    V.push_back(column);
    indexChanged.push_back(V);
    qDebug() << indexChanged.size();
    QString newValue = item(row,column)->text();

    qDebug() << VM.size();
    if(VM.size()>0){
        qDebug() << VM[0].size();
    }

    //Backup nilai sebelum diedit
    if(VM.size() == 0){
        oldCellText.push_back("");
        qDebug("VM empty");
        VM.resize(row+1);
        VM[row].resize(column+1);
    }
    else if((unsigned)row < VM.size() && (unsigned)column < VM[0].size()){
        if(varTypes.at(column) == "String"){
            oldCellText.push_back(VM[row][column].string);
        }else{
            oldCellText.push_back(QString::number(VM[row][column].real).toStdString());
        }
    }
    else if((unsigned)row >= VM.size() && (unsigned)column < VM[0].size()){
        oldCellText.push_back("");
        VM.resize(VM.size()+1, std::vector<type> (VM[0].size()));
        for(unsigned j=0; j< VM[0].size(); j++){
            if(varTypes.at(j) != "String"){
                VM[row][j].real = std::numeric_limits<double>::quiet_NaN();
            }
        }
    }else if((unsigned)column >= VM[0].size() && (unsigned)row < VM.size()){
        oldCellText.push_back("");
        //VM.resize(VM.size(), std::vector<type> (VM[0].size()+1));
        for(unsigned i=0; i< VM.size(); i++){
            VM[i].resize(column+1);
            if(varTypes.at(column) != "String"){
                VM[i][column].real = std::numeric_limits<double>::quiet_NaN();
            }
        }
    }else if((unsigned)column >= VM[0].size() && (unsigned)row >= VM.size()){
        oldCellText.push_back("");
        VM.resize(row+1);
        for(unsigned i=0; i< VM.size(); i++){
            VM[i].resize(column+1);
            if(varTypes.at(column) != "String"){
                VM[i][column].real = std::numeric_limits<double>::quiet_NaN();
            }
        }
        for(unsigned j=0; j< VM[0].size(); j++){
            if(varTypes.at(j) != "String"){
                VM[row][j].real = std::numeric_limits<double>::quiet_NaN();
            }
        }
    }
    qDebug("--");
    qDebug() << VM.size();
    qDebug() << VM[0].size();

    //Ubah nilai Vector setelah diedit
    if(varTypes.at(column) == "String"){
        if(newValue == ""){
            VM[row][column].string = "NA";
        }else{
            VM[row][column].string = newValue.toStdString();
        }
    }else{
        if(newValue == ""){
            VM[row][column].real = std::numeric_limits<double>::quiet_NaN(); //Ubah nilai Vector dengan 'nan'
        }else{
            VM[row][column].real = newValue.toDouble();
        }
    }
    qDebug("000");
    ((MainWindow*) QApplication::activeWindow())->setWindowModified(true);
    qDebug("<<<");
    blockSignals(false);
}

void DataEditor::keyPressEvent(QKeyEvent* event)
{
    //Saat enter ganti baris, tapi saat edit cell belum bisa tekan tombol kiri atau kanan untuk pindah kolom
    //qDebug() << event->key();
    if(event->key() == 16777220) //Key_Enter = 16777221
    {
        setCurrentCell(currentRow()+1,currentColumn());
    }
    else
    {
        QTableWidget::keyPressEvent(event);
    }
}

std::vector<std::vector<int> > DataEditor::getIndexChanged()
{
    //Urutkan dulu berdasarkan kolomnya
    if(indexChanged.size()>0){
        std::vector<int> cols;
        std::vector<bool> order;
        cols.push_back(indexChanged[0].size()-1);
        order.push_back(true);
        DataManagement *dm = new DataManagement;
        dm->mergeSortInt(indexChanged,cols,order,0,indexChanged.size()-1);
        delete dm;
    }
    return indexChanged;
}

void DataEditor::clearIdxChanged()
{
    qDebug("Clear Index Changed");
    indexChanged.clear();
    ((MainWindow*) QApplication::activeWindow())->setWindowModified(false);
    qDebug("------");
}

void DataEditor::cellValidation(int row, int column)
{
    //Validasi saat ada perubahan nilai di cell, berdasarkan type (decimal, missing dan scale belum dibuat)
    qDebug("Validasi");
    QRegExp re("[+-]?\\d*\\.?\\d+");
    if(item(row, column)->text() != "" && column < varTypes.size()){
        QString string = item(row,column)->text();
        QString type;

        if(!re.exactMatch(string)){
            //string
            type ="String";
        }else{
            //numeric
            bool validate;
            string.toInt(&validate);
            if(validate == true){
                type ="Integer";
                goto check;
            }
            string.toDouble(&validate);
            if(validate == true){
                type ="Real";
            }
        }
        check:
        //qDebug(type.toLocal8Bit());
        if(varTypes[column] == type || (varTypes[column] == "Real" && type == "Integer")){
            qDebug("Sama");
            if((unsigned) row < VM.size()){
                addIndexChanged(row,column);
            }else if((unsigned) row == VM.size()){
                qDebug("add row");
                //tambahkan ke indexChanged dan ubah vectornya
                addIndexChanged(row,column);

                //Update ke Variable Editor
                ((MainWindow*) QApplication::activeWindow())->addNewRow();
            }else{
                goto forbid;
            }
        }else {
            qDebug("Beda");
            item(row,column)->setText("");
            setCurrentCell(row,column);
            if(varTypes[column]=="String"){
                if(type == "Integer" || type == "Real"){
                    qDebug("Tipe Variabel String");
                    //QMessageBox::information(0,"Info","Tipe Variabel String");
                }
            }else if(varTypes[column]=="Integer"){
                if(type == "String" || type == "String"){
                    qDebug("Tipe Variabel Integer");
                    //QMessageBox::information(0,"Info","Tipe Variabel Integer");
                }
            }else if(varTypes[column]=="Real"){
                if(type == "Integer" || type == "String"){
                    qDebug("Tipe Variabel Real");
                    //QMessageBox::information(0,"Info","Tipe Variabel Real");
                }
            }
        }
    }else{
        if(column == varTypes.size()){
            qDebug("Add new Column");

            if((unsigned) row == VM.size()){
                qDebug("add row");
                //Update ke Variable Editor
                ((MainWindow*) QApplication::activeWindow())->addNewRow();
            }else if((unsigned) row > VM.size()){
                goto forbid;
            }

            //add vartypes
            QString string = item(row,column)->text();
            QString type;

            if(!re.exactMatch(string)){
                //string
                type ="String";
            }else{
                //numeric
                bool validate;
                string.toInt(&validate);
                if(validate == true){
                    type ="Integer";
                    goto addNewType;
                }
                string.toDouble(&validate);
                if(validate == true){
                    type ="Real";
                }
            }
            addNewType:
            varTypes.push_back(type);
            varNames.push_back(horizontalHeaderItem(column)->text());

            //tambahkan ke indexChanged dan ubah vectornya
            addIndexChanged(row,column);
            //Update ke Variable Editor
            ((MainWindow*) QApplication::activeWindow())->addNewVarVE(column, horizontalHeaderItem(column)->text(), type);

        }
        else{
            forbid:
            item(row,column)->setText("");
            setCurrentCell(row,column);
        }
    }
}

void DataEditor::setVM(std::vector<std::vector<type> > VM1)
{
    qDebug("setVM");
    VM = VM1;
}

std::vector<std::vector<type> > DataEditor::getVM()
{
    qDebug("getVM");
    if(isSorted && VMsorted.size()>0){
        //return VM yg telah diurutkan
        qDebug("isSorted");
        std::vector<std::vector<type> > VM1(VM.size(), std::vector<type> (VM[0].size()));
        for(unsigned i=0; i< VM.size(); i++){
            int index = i;
            if(isSorted && VMsorted.size()>0){
                index = VMsorted[i][0].real;
            }
            VM1[i] = VM[index];
        }
        VM = VM1;
        isSorted = false;
    }
    return VM;
}

void DataEditor::setVMsorted(std::vector<std::vector<type> > VM1)
{
    VMsorted = VM1;
    isSorted = true;
}

QStringList DataEditor::getVarTypes()
{
    return varTypes;
}

void DataEditor::reloadTable(bool colNames, bool sorted1)
{
    //Reload table
    isSorted = sorted1;
    isReload = true;
    blockSignals(true);
    qDebug("Reload Tabel");
    clearContents();
    for (unsigned j=0; j<VM[0].size(); j++) {
        int row = VM.size();
        if(row > 10000){
            row = 50;
            setRowCount(row);
        }else{
            setRowCount(row+20);
        }

        for (int i=0; i<row; i++) {
            int index = i;
            if(isSorted && VMsorted.size()>0){
                index = VMsorted[i][0].real;
            }
            QString string;
            if(varTypes[j]=="String"){
                string =QString::fromStdString(VM[index][j].string);
            }else{
                string = QString::number(VM[index][j].real);
            }
            if(string == "nan" || string.isEmpty()){
                string = "NA";
            }

            QTableWidgetItem *item = new QTableWidgetItem;
            item->setText(string);
            item->setTextAlignment(Qt::AlignCenter);
            setItem(i,j,item);
        }
    }
    if(colNames){
        for (int i = 0; i < varNames.size(); ++i) {
            QTableWidgetItem *item = new QTableWidgetItem;
            item->setText(varNames[i]);
            setHorizontalHeaderItem(i, item);
        }
        for (int i = varNames.size(); i < columnCount(); ++i) {
            QTableWidgetItem *item = new QTableWidgetItem;
            item->setText("V" + QString::number(i+1));
            setHorizontalHeaderItem(i, item);
        }
    }
    idxScroll = 0;
    blockSignals(false);

}

void DataEditor::loadVarReal(std::vector<type> V, int col, QString varName, QString varType)
{
    //Reload satu kolom tipe numeric
    blockSignals(true);
    //reload varNames
    isReload = true;
    if(col < varNames.size()){
        varNames.replace(col,varName);
        varTypes.replace(col,varType);
    }else{
        varNames.insert(col,varName);
        varTypes.insert(col,varType);
    }
    QTableWidgetItem *item = new QTableWidgetItem;
    item->setText(varNames.at(col));
    setHorizontalHeaderItem(col, item);

    //load variable real
    qDebug("Load Variabel");
    int row = V.size();
    if(row > 10000){
        row =50;
    }
    for (int i=0; i< row; i++) {
        QString string = QString::number(V[i].real);
        if(string == "nan" || string.isEmpty()){
            string = "NA";
        }
        QTableWidgetItem *item = new QTableWidgetItem;
        item->setText(string);
        item->setTextAlignment(Qt::AlignCenter);
        setItem(i,col,item);
    }

   /*
    for(int i=0; i<varNames->size(); i++){
        QMessageBox::information(0,"",varNames->at(i));
    }*/
    blockSignals(false);
}

void DataEditor::loadVarStr(std::vector<type> V, int col, QString varName, QString varType)
{
    //reload satu kolom tipe string
    blockSignals(true);
    //reload varNames
    isReload = true;
    if(col < varNames.size()){
        varNames.replace(col,varName);
        varTypes.replace(col,varType);
    }else{
        varNames.insert(col,varName);
        varTypes.insert(col,varType);
    }
    QTableWidgetItem *item = new QTableWidgetItem;
    item->setText(varNames.at(col));
    setHorizontalHeaderItem(col, item);

    qDebug("Load Variabel");
    int row = V.size();
    if(row > 10000){
        row =50;
    }
    for (int i=0; i<row; i++) {
        QString string = QString::fromStdString(V[i].string);
        if(string == "nan" || string.isEmpty()){
            string = "NA";
        }
        QTableWidgetItem *item = new QTableWidgetItem;
        item->setText(string);
        item->setTextAlignment(Qt::AlignCenter);
        setItem(i,col,item);
    }
    blockSignals(false);
}


void DataEditor::showRowContextMenu(const QPoint& pos)
{
    qDebug("Click");
    int row = verticalHeader()->logicalIndexAt(pos);
    if(item(row,0)){ //Belum dicek kalau item text perbarisnya kosong(" ")
        QPoint globalPos = mapToGlobal(pos);
        QMenu myMenu;
        myMenu.addAction("Insert Row");
        myMenu.addAction("Delete Row");
        QAction* selectedItem = myMenu.exec(globalPos);
        if(selectedItem){
            if (selectedItem->text() == "Delete Row"){
                deleteRow(row);
            }else if(selectedItem->text() == "Insert Row"){
                insertNewRow(row);
            }
        }
    }
}

void DataEditor::showColContextMenu(const QPoint& pos)
{
    qDebug("Click");
    int col = horizontalHeader()->logicalIndexAt(pos);
    if(item(0,col)){ //Belum dicek kalau item text perbarisnya kosong(" ")
        QPoint globalPos = mapToGlobal(pos);
        QMenu myMenu;
        myMenu.addAction("Insert Variable");
        myMenu.addAction("Delete Variable");
        QAction* selectedItem = myMenu.exec(globalPos);
        if(selectedItem){
            qDebug() << selectedItem->text().toLocal8Bit();
            if (selectedItem->text() == "Delete Variable"){
                //Update lewat Var Editor
                ((MainWindow*) QApplication::activeWindow())->deleteVarVE(col);//update ke Active dataset dan variabel editor
            }else if(selectedItem->text() == "Insert Variable"){
                //Update lewat Var Editor
                QString string = "NewVar"+QString::number(col+1);
                ((MainWindow*) QApplication::activeWindow())->addNewVarVE(col,string,"String");//update ke Active dataset dan variabel editor
            }
        }
    }
}
void DataEditor::deleteVar(int col)
{
    qDebug("DeleteVar");
    qDebug() << col;

    for(unsigned i=0; i < VM.size(); i++){
        addIndexChanged(i,col);
    }

    removeColumn(col); //delete tabel
    qDebug() << VM[0].size();
    for(unsigned i=0; i< VM.size(); i++){
        VM[i].erase(VM[i].begin()+col);//delete vector
    }
    qDebug() << VM[0].size();
}

void DataEditor::deleteRow(int row)
{
    qDebug("Delete row");
    qDebug() << row;

    for(unsigned i=0; i < VM[0].size(); i++){
        addIndexChanged(row,i);
    }

    removeRow(row); //delete tabel
    qDebug() << VM.size();
    VM.erase(VM.begin()+row); //delete vector
    qDebug() << VM.size();
    //Update ke Active Dataset
    ((MainWindow*) QApplication::activeWindow())->deleteRow(1); //jumlah row yg dihapus
}

void DataEditor::insertNewRow(int row)
{
    qDebug("Insert row");
    qDebug() << row;

    for(unsigned i=0; i < VM[0].size(); i++){
        addIndexChanged(row,i);
    }

    insertRow(row); //insert tabel
    QTableWidgetItem *it;

    qDebug() << VM.size();
    VM.insert(VM.begin()+row,VM.begin()+row,VM.begin()+row+1); //insert vector
    qDebug() << VM.size();

    type t;
    t.real = std::numeric_limits<double>::quiet_NaN();
    t.string = "NA";
    for(unsigned j=0; j<VM[0].size(); j++){
        VM[row][j] = t;
        it = new QTableWidgetItem;
        it->setText("");
        setItem(row,j,it);
    }

    for(unsigned i=0; i<VM.size(); i++){
        qDebug() << QString::number(VM[i][1].real).toLocal8Bit();
    }
    //Update ke Active Dataset
    ((MainWindow*) QApplication::activeWindow())->deleteRow(-1); //jumlah row yg dihapus, kalau minus, jumlah row y ditambah
}

void DataEditor::insertNewVar(int col)
{
    qDebug("---");
    qDebug() << VM[0].size();
    qDebug() << varTypes.size();
    qDebug() << varNames.size();
    qDebug("Insert Var");
    qDebug() << col;

    for(unsigned i=0; i < VM.size(); i++){
        addIndexChanged(i,col);
    }

    insertColumn(col); //insert tabel
    QTableWidgetItem *it = new QTableWidgetItem;
    it->setText("NewVar"+QString::number(col+1));
    setHorizontalHeaderItem(col,it);

    qDebug() << VM[0].size();
    type t;
    t.real = std::numeric_limits<double>::quiet_NaN();
    t.string = "NA";
    for(unsigned i=0; i< VM.size(); i++){
        VM[i].insert(VM[i].begin()+col,VM[i].begin()+col,VM[i].begin()+col+1); //insert vector
        VM[i][col] = t;
        it = new QTableWidgetItem;
        it->setText("");
        setItem(i,col,it);
    }
    qDebug() << VM[0].size();

    for(unsigned i=0; i<VM[0].size(); i++){
        qDebug() << QString::number(VM[0][i].real).toLocal8Bit();
    }
}
