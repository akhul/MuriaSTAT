#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "open.h"
//==UPDATE==//
#include "slregression.h"
#include "regressionr.h"
#include "qwebframe.h"
//Update 27 Juni
#include "dmsortingdialog.h"
#include "dmrecodedialog.h"
#include "dmaggregatedialog.h"
#include "dm_mergebycoldialog.h"

MainWindow::MainWindow(QWidget *parent, RInside &m_R) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    R(m_R)
{

    ui->setupUi(this);
    MainWindow::showMaximized();
    setCurrentFile("");
    //intialization variable
    vSplitter1=new QSplitter(Qt::Vertical);
    vSplitter2=new QSplitter(Qt::Vertical);
    vSplitterMain=new QSplitter(Qt::Vertical);
    hSplitter=new QSplitter(Qt::Horizontal);
    rCoreClass = new RCore(R);
    openClass = new Open;
    dataEditor=new DataEditor;
    variableEditor=new VariableEditor;

    activeDatasetClass = new ActiveDataset(this, dataEditor, variableEditor);
    activeDatasetClass->setRcore(rCoreClass);
    activeDatasetClass->setMainText(ui->mainPlainTextEdit);
    activeDatasetClass->setMainEditor(ui->mainEditor);
    activeDatasetClass->setMainOutput(ui->mainOutput);

    save = new Save(R);
    save->setActiveDataSet(activeDatasetClass);
    save->setMainWindow(this);
    save->setRCoreClass(rCoreClass);

    //Design MainWindow
    createContentMainWindow();

    //intial active dataset
    intialActiveDataset();

    //register Events
    createActions();

    //declaration everything is needed

}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createContentMainWindow(){

    setOutputDesign();

    vSplitterMain->addWidget(ui->mainPlainTextEdit);
    vSplitterMain->addWidget(ui->mainEditor);
    vSplitterMain->addWidget(ui->mainOutput);

    vSplitter2->addWidget(ui->datasetGroupBox);
    vSplitter2->addWidget(ui->extraTabWidget);


    vSplitter1->addWidget(vSplitterMain);
    vSplitter1->setContentsMargins(4,1,2,1);
    hSplitter->addWidget(vSplitter1);
    hSplitter->addWidget(vSplitter2);
    setCentralWidget(hSplitter);

    QList<int> weights;
    weights.push_back(900);
    weights.push_back(360);
    hSplitter->setSizes(weights);

    ui->mainPlainTextEdit->insertPlainText("Welcome To Muria");

    //Update 26 Juni
    showMainWidget(0);
    //Submenu yg tidak aktif saat belum ada data yg diload atau data editor belum dibuka
    ui->actionSave->setEnabled(false);
    ui->actionSave_As->setEnabled(false);

    ui->actionSort->setEnabled(false);
}
void MainWindow::createActions()
{
    connect(ui->actionDataset, SIGNAL(triggered()), this, SLOT(setWindowShow()));
    connect(ui->actionExtras, SIGNAL(triggered()), this, SLOT(setWindowShow()));
    connect(ui->actionData, SIGNAL(triggered()), this, SLOT(openNewData()));
    connect(ui->actionLinear_Regression,SIGNAL(triggered()),this,SLOT(SlRegressionDialog()));
    //update 27 juni
    connect(ui->actionNewData, SIGNAL(triggered()),this, SLOT(newData()));
    connect(ui->menuBar, SIGNAL(hovered(QAction*)),this, SLOT(enableActionSave()));
    connect(ui->actionSave, SIGNAL(triggered()),this, SLOT(saveFile()));
    connect(ui->actionSave_As, SIGNAL(triggered()), this, SLOT(saveAs()));
    connect(ui->actionSort, SIGNAL(triggered()), this, SLOT(sort()));

    //Update 29 Juni
    connect(ui->actionShowHome, SIGNAL(triggered()), this, SLOT(setWindowShow()));
    connect(ui->actionShowOutput, SIGNAL(triggered()), this, SLOT(setWindowShow()));
    connect(ui->actionShowData_Editor, SIGNAL(triggered()), this, SLOT(setWindowShow()));
    connect(ui->actionRecodeSameVar, SIGNAL(triggered()), this, SLOT(recodeSameVar()));
    connect(ui->actionAggregate, SIGNAL(triggered()), this, SLOT(aggregate()));
    connect(ui->actionMerge_by_Column, SIGNAL(triggered()), this, SLOT(mergeByCol()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(exit()));

  }

void MainWindow::setWindowShow()
{
    if(ui->actionShowHome->isChecked()){
        showMainWidget(0);
    }else if(ui->actionShowData_Editor->isChecked()){
        showMainWidget(1);
    }else if(ui->actionShowOutput->isChecked()){
        showMainWidget(2);
    }


    if(ui->actionDataset->isChecked()){
       ui->datasetGroupBox->resize(360,ui->datasetGroupBox->height());
       ui->datasetGroupBox->show();
    }else{
       ui->datasetGroupBox->hide();
    }

    if(ui->actionExtras->isChecked()){
       ui->extraTabWidget->resize(360,ui->extraTabWidget->height());
       ui->extraTabWidget->show();
    }else{
       ui->extraTabWidget->hide();
    }
}

void MainWindow::openNewData(){
    //Update 29 Juni
    openClass->openNewData(rCoreClass,activeDatasetClass);

    if(!openClass->getFileName().isEmpty()){
        //Submenu yg aktif saat ada data yg diload
        ui->actionSort->setEnabled(true);
    }
}

void MainWindow::intialActiveDataset(){
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(activeDatasetClass);
    ui->datasetGroupBox->setLayout(mainLayout);

}
void MainWindow::setCurrentFile(const QString &fileName2)
{
    //qDebug("setcurrent file");
    fileName = fileName2;
    setWindowModified(false);

    QString shownName = tr("");
    if (!fileName.isEmpty()) {
        shownName = strippedName(fileName);
        setWindowTitle(tr("%1[*] - %2").arg(shownName).arg(tr("MuriaSTAT")));
    }else{
        setWindowTitle(tr("%1[*]%2").arg(shownName).arg(tr("MuriaSTAT")));
    }
    //qDebug() << windowTitle().toLocal8Bit();
}
QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}


//====Update===//
void MainWindow::SlRegressionDialog(){
    saveToR(); //Kalau ada proses DM sebelumnya, save dataFrame di dataeditor ke R
    SLRegression *sLRegDialog = new SLRegression(0,R);
    sLRegDialog->setR(R);
    sLRegDialog->setCBDatasetName(activeDatasetClass->getDatasetNameList());
    sLRegDialog->show();

    if(sLRegDialog->exec()==QDialog::Accepted){
        RegressionR *reg = new RegressionR(R);
        reg->setStatIndicator(sLRegDialog->getStatIndicator());
        QMessageBox::information(this,"",sLRegDialog->getStatIndicator().join("+")+QString::fromStdString(sLRegDialog->getModel()));
        reg->setOuputHtml(sLRegDialog->getModel(),sLRegDialog->getDatasetName());

        QString datasetName = QString::fromStdString(sLRegDialog->getDatasetName());
        QString analysis = "Regresi";
        QStringList compList;
        compList.append("Summary Model:");
        compList.append("Coefficient:");
        compList.append("Residual:");
        addTreeOutputItem(datasetName, analysis, compList); //Untuk menambahkan treewidget Output, judul datset dan analisis pada outputWebView (buka methodnya biar tau)
        addOutput(reg->getAllHtml()); //Untuk menambahkan html hasil analisis pada output
        showMainWidget(2); //Untuk menampilkan output
    }
}

// ----Update 27 Juni
//0=PlainText, 1=Editor, 2=Output
void MainWindow::showMainWidget(int i){
    if(i==0){
        saveToR();
        ui->mainPlainTextEdit->show();
        ui->mainEditor->hide();
        ui->mainOutput->hide();
        setCurrentFile("");
    }
    else if(i==1){
        ui->mainPlainTextEdit->hide();
        ui->mainEditor->show();
        if(!ui->mainEditor->layout()){
            qDebug("-gak ada layout-");
            newData();
        }
        ui->mainOutput->hide();
    }else if(i==2){
        saveToR();
        ui->mainPlainTextEdit->hide();
        ui->mainEditor->hide();
        ui->mainOutput->show();
        setCurrentFile("");
    }
    ui->actionShowHome->setChecked(false);
    ui->actionShowData_Editor->setChecked(false);
    ui->actionShowOutput->setChecked(false);
    setWindowModified(false);
}

//Update 27 Juni
void::MainWindow::setOutputDesign()
{
    //Mengatur design dan widget pada output (treewidget dan webview)

    QSplitter *splitter=new QSplitter(Qt::Horizontal);
    treeOutput = new QTreeWidget;
    outputWV = new QWebView;
    treeOutput->header()->hide();
    outputWV->setHtml("Output");
    outputWV->setStyleSheet("selection-background-color: lightblue;");

    connect(treeOutput,SIGNAL(clicked(QModelIndex)),this,SLOT(scrollOutput()));
    splitter->addWidget(treeOutput);
    splitter->addWidget(outputWV);

    splitter->setStretchFactor(1, 1);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(splitter);
    ui->mainOutput->setLayout(layout);

    //Add treeOutput and OutputWebView
    QStringList componentList;
    componentList.append("Component1");
    componentList.append("Component2");
    componentList.append("Component3");

    addTreeOutputItem("Dataset1","Analysis1",componentList);
    addTreeOutputItem("Dataset1","Regresi",componentList);
    addTreeOutputItem("Dataset2","Analysis1",componentList);
    addTreeOutputItem("Dataset1","Regresi",componentList);
    treeOutput->expandAll();

    addOutput("Dataset1 <br><br><br><br><br><br><br><br> Analysis1 <br><br><br><br><br><br><br><br> Component1 "
              "<br><br><br><br><br><br><br><br> Component2<br><br><br><br><br><br><br><br> Component3"
              "<br><br><br><br><br><br><br><br> Regresi <br><br><br><br><br><br><br><br> Component1 "
              "<br><br><br><br><br><br><br><br> Component2<br><br><br><br><br><br><br><br> Component3"
              "<br><br><br><br><br><br><br><br> Regresi <br><br><br><br><br><br><br><br> Component1 "
              "<br><br><br><br><br><br><br><br> Component2<br><br><br><br><br><br><br><br> Component3"
              "<br><br><br><br><br><br><br><br>Dataset2 <br><br><br><br><br><br><br><br> Analysis1 "
              "<br><br><br><br><br><br><br><br> Component1 "
              "<br><br><br><br><br><br><br><br> Component2<br><br><br><br><br><br><br><br> Component3");
    clearOutput();
}

void MainWindow::setDataEditorConnect(DataEditor *DE)
{
    //Set connect pada Data Editor dari menu di mainWindow
    dataEditor = DE;
    connect(ui->actionCut, SIGNAL(triggered()), dataEditor, SLOT(cut()));
    connect(ui->actionCopy, SIGNAL(triggered()), dataEditor, SLOT(copy()));
    connect(ui->actionPaste, SIGNAL(triggered()), dataEditor, SLOT(paste()));
    connect(ui->actionDelete, SIGNAL(triggered()), dataEditor, SLOT(del()));

}

void MainWindow::newData()
{
    //Membuka data editor baru

    //Submenu yg aktif saat data editor dibuka
    //ui->actionSort->setEnabled(true);

    QString fileName = "NewData.";
    disableActionSave();
    setCurrentFile(fileName);
    dataEditor = activeDatasetClass->getDataEditor();
    if(dataEditor){
        delete dataEditor;
    }
    dataEditor = new DataEditor;
    dataEditor->setScrollConnect();

    connect(ui->actionCut, SIGNAL(triggered()), dataEditor, SLOT(cut()));
    connect(ui->actionCopy, SIGNAL(triggered()), dataEditor, SLOT(copy()));
    connect(ui->actionPaste, SIGNAL(triggered()), dataEditor, SLOT(paste()));
    connect(ui->actionDelete, SIGNAL(triggered()), dataEditor, SLOT(del()));

    variableEditor = activeDatasetClass->getVarEditor();
    if(variableEditor){
        delete variableEditor;
    }
    variableEditor = new VariableEditor;
    variableEditor->setActiveDataset(activeDatasetClass);
    variableEditor->setDataEditor(dataEditor);

    activeDatasetClass->setDataEditor(dataEditor);
    activeDatasetClass->setVarEditor(variableEditor);

    TabEditor *newTab = new TabEditor;
    newTab->newEditor(dataEditor, variableEditor);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(newTab);
    if(ui->mainEditor->layout()){
        qDebug("ada layout");
        delete ui->mainEditor->layout();
    }else{
        qDebug("gak ada layout");
        delete ui->mainEditor->layout();
    }
    ui->mainEditor->setLayout(layout);

    showMainWidget(1);
}

void MainWindow::addTreeOutputItem(QString dataset, QString analysis, QStringList componentList)
{
    //Menambahkan item pada output treeWidget
    QList<QTreeWidgetItem *> datasetList = treeOutput->findItems(dataset,Qt::MatchCaseSensitive);
    if(datasetList.size()>0){ //jika dataset sudah ada di treewidget sebelumnya, tambahkan analisis di dataset tersebut
        addOutput("Analysis: "+analysis); //menambahkan nama analisis pada outputWebView

        int d=1;
        for(int i=0; i< datasetList.first()->childCount(); i++){
            if(datasetList.first()->child(i)->text(0) == analysis){
                d++;
            }
        }
        QTreeWidgetItem *analysisTWI = new QTreeWidgetItem(datasetList.first());
        analysisTWI->setText(0,analysis);
        analysisTWI->setStatusTip(0,QString::number(d));

        for(int i=0; i<componentList.size(); i++){
            QTreeWidgetItem *compTWI = new QTreeWidgetItem(analysisTWI);
            compTWI->setText(0,componentList[i]);
            compTWI->setStatusTip(0,"1");
        }
    }else{
        addOutput("Dataset: "+dataset); //menambahkan nama dataset pada outputWebView
        addOutput("Analysis: "+analysis); //menambahkan nama analisis pada outputWebView

        QTreeWidgetItem *dataSetTWI = new QTreeWidgetItem(treeOutput);
        dataSetTWI->setText(0,dataset);
        dataSetTWI->setStatusTip(0,"1");

        QTreeWidgetItem *analysisTWI = new QTreeWidgetItem(dataSetTWI);
        analysisTWI->setText(0,analysis);
        analysisTWI->setStatusTip(0,"1");
        for(int j=0; j< componentList.size(); j++){
            QTreeWidgetItem *componentTWI = new QTreeWidgetItem(analysisTWI);
            componentTWI->setText(0,componentList[j]);
            componentTWI->setStatusTip(0,"1");
        }
    }
    treeOutput->expandAll();
}

void MainWindow::saveAs()
{
    if(ui->mainOutput->isHidden()){
        save->saveAs();
    }else{
        printToPDF();
    }
}

void MainWindow::saveFile()
{
    if(ui->mainOutput->isHidden()){
        save->saveFile();
    }else{
        printToPDF();
    }
}

void MainWindow::enableActionSave()
{
    //Mengaktifkan submenu save saat da perubahan di dataset yg aktif (Masih belum pas, kalau dataset sudah disimpan ke R, diangap sudah tidak ada perubahan)
    if((!ui->actionSave->isEnabled() && isWindowModified()) || !ui->mainOutput->isHidden()){
        ui->actionSave->setEnabled(true);
        ui->actionSave_As->setEnabled(true);
    }
}

void MainWindow::disableActionSave()
{
    //Menonaktifkan submenu save saat belum ada perubahan di dataset yg aktif
    setWindowModified(false);
    ui->actionSave->setEnabled(false);
    ui->actionSave_As->setEnabled(false);
}

void MainWindow::showSaveDialog(QString dataName)
{
    //Sebelum memanggil method ini, harus dipastikan sebelumnya apakah ada perubahan atau tidak di dataset
    QMessageBox::StandardButton saveMessage;
    saveMessage = QMessageBox::question(this, dataName+" modified", "Save Changes?",
                                    QMessageBox::Yes|QMessageBox::No);
    if (saveMessage == QMessageBox::Yes) {
        qDebug("yes");
        save->saveFile();
    }
}

void MainWindow::saveToR()
{
    //Cek ada perubahan apa gak, kalo ada tapi belum dibuat dataset, tawari save. Jika udah ada dataset, langsung save ke R
    qDebug("main window - save to R");

    dataEditor = activeDatasetClass->getDataEditor();
    QString title = windowTitle().split(".").at(0);
    QStringList datasetList = activeDatasetClass->getDatasetNameList();
    bool ada = false;
    for(int i=0; i< datasetList.size(); i++){
        if(datasetList[i] == title){
            ada = true;
            //activeDatasetClass->updateDSModifiedList(i);
        }
    }
    if(!ada){ //Belum dibuat dataset
        if(isWindowModified()){
            showSaveDialog(title);
        }
        return;
    }

    if(dataEditor->isReload || isWindowModified()){
        save->saveToR(); // save ke R
    }
}

void MainWindow::scrollOutput()
{
    //Mencari textt di output Webview sesuai dengan item yg diklik pada treeWidget
    outputWV->findText("");
    QStringList selectedTree;
    QList<QTreeWidgetItem *> selected_rows = treeOutput->selectedItems();

    if(selected_rows.first()->statusTip(0) != ""){
        for(int i=0; i<selected_rows.first()->statusTip(0).toInt();i++){
            selectedTree.prepend(selected_rows.first()->text(0));
        }
    }

    if(selected_rows.first()->parent()){
        if(selected_rows.first()->parent()->statusTip(0) != ""){
            for(int i=0; i<selected_rows.first()->parent()->statusTip(0).toInt();i++){
                selectedTree.prepend(selected_rows.first()->parent()->text(0));
            }
        }
        if(selected_rows.first()->parent()->parent()){
            selectedTree.prepend(selected_rows.first()->parent()->parent()->text(0));
        }
    }
    for(int i=0; i<selectedTree.size(); i++){
        outputWV->findText(selectedTree[i],QWebPage::FindCaseSensitively);

    }

}

void MainWindow::addOutput(QString outputText)
{
    //Menambahkan isi output satu baris di bawahnya
    QString tempOutput = outputWV->page()->currentFrame()->toHtml();
    outputWV->setHtml(tempOutput +"<br>"+outputText);
}

void MainWindow::clearOutput()
{
    //Menghapus isi output
    outputWV->setHtml("Output"); //Isinya hanya tulisan "Output", silahkan diganti yg lebih pas

    //menghapus isi treewidget
    treeOutput->clear();
}

void MainWindow::addNewVarVE(int col, QString newVar, QString newType)
{
    //Menambahkan variabel baru di Variabel editor, method ini ditrigger dari penambahan variabel baru di Data Editor
    variableEditor = activeDatasetClass->getVarEditor();
    variableEditor->addNewVarVE(col, newVar, newType);
    setWindowModified(true);
}

void MainWindow::deleteVarVE(int col)
{
    //Menambahkan variabel baru di Variabel editor, method ini ditrigger dari penambahan variabel baru di Data Editor
    variableEditor = activeDatasetClass->getVarEditor();
    variableEditor->deleteVarVE(col);
    setWindowModified(true);
}

void MainWindow::deleteRow(int rowCount){
    //Update active dataset
    QString datasetName = windowTitle().split(".").at(0);
    int index = activeDatasetClass->getDatasetIndex(datasetName);
    if(index != -1){
        QString path = activeDatasetClass->getPathList().at(index);
        QString extension = activeDatasetClass->getExtList().at(index);
        Rcpp::StringVector colNames;
        dataEditor = activeDatasetClass->getDataEditor();
        for(int c=0; c<dataEditor->getVarTypes().size(); c++){
            colNames.push_back(dataEditor->horizontalHeaderItem(0)->text().toStdString());
        }
        int rowIndex = activeDatasetClass->getDatasetRowIndex(datasetName);
        if(rowIndex != -1){
            int row =activeDatasetClass->item(rowIndex,3)->text().split("and ").at(1).split(" Observation").at(0).toInt();
            row = row - rowCount;
            int col = colNames.size();
            activeDatasetClass->updateRow(rowIndex,colNames,datasetName,datasetName,extension,path,col,row);
        }
    }
    setWindowModified(true);
}

void MainWindow::addNewRow()
{
    //Update active dataset
    QString datasetName = windowTitle().split(".").at(0);
    int index = activeDatasetClass->getDatasetIndex(datasetName);
    if(index != -1){
        QString path = activeDatasetClass->getPathList().at(index);
        QString extension = activeDatasetClass->getExtList().at(index);
        Rcpp::StringVector colNames;
        dataEditor = activeDatasetClass->getDataEditor();
        for(int c=0; c<dataEditor->getVarTypes().size(); c++){
            colNames.push_back(dataEditor->horizontalHeaderItem(0)->text().toStdString());
        }
        int rowIndex = activeDatasetClass->getDatasetRowIndex(datasetName);
        if(rowIndex != -1){
            int row =activeDatasetClass->item(rowIndex,3)->text().split("and ").at(1).split(" Observation").at(0).toInt();
            row++;
            int col = colNames.size();
            activeDatasetClass->updateRow(rowIndex,colNames,datasetName,datasetName,extension,path,col,row);
        }
    }
    setWindowModified(true);
}

void MainWindow::exit()
{
    saveToR(); //Cek ada perubahan apa gak, kalo ada tapi belum dibuat dataset, tawari save. Jika udah ada dataset, langsung save ke R

    QStringList datasetNameList = activeDatasetClass->getDatasetNameList();
    for(int i=0; i<datasetNameList.size(); i++){
        if(activeDatasetClass->getDSModifiedAt(i)){ //cek apakah dataset ke-i ada perubahan dari awal load
            showSaveDialog(datasetNameList[i]); // Bila ada perubahan di setiap dataset tawari save
        }
    }
    close();
}

void MainWindow::printToPDF()
{
    // Set location of resulting PDF
    QString path = QFileDialog::getSaveFileName(this,"Save Output",".",
                                              "PDF files (*.pdf)"
                                              ";;All files (*.*)");
    // Initialize printer and set save location
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFileName(path);

    // Create PDF
    outputWV->print(&printer);
}

void MainWindow::sort()
{
    saveToR(); //Kalau ada proses DM sebelumnya, save dataFrame di dataeditor ke R

    QStringList DSList = activeDatasetClass->getDatasetNameList();
    DMSortingDialog *dialog = new DMSortingDialog(this,R);
    dialog->setDE(dataEditor);
    dialog->setActiveDS(activeDatasetClass);
    if(DSList.size()>0){
        dialog->setCBDatasetName(DSList);
        for(int i=0; i<DSList.size(); i++){
            if(DSList[i] == windowTitle().split(".").at(0)){
                dialog->setDataDisplayedName(DSList[i]);
                dialog->setCurrentCBDSName(i);
            }
        }
    }else{
        //DSList.append(windowTitle().split(".").at(0));
        //dialog->setDataDisplayedName(DSList[0]);
    }
    dialog->show();
}

void MainWindow::recodeSameVar()
{
    saveToR(); //Kalau ada proses DM sebelumnya, save dataFrame di dataeditor ke R

    QStringList DSList = activeDatasetClass->getDatasetNameList();
    DMRecodeDialog *dialog = new DMRecodeDialog(this,R);
    dialog->setDE(dataEditor);
    dialog->setActiveDS(activeDatasetClass);
    if(DSList.size()>0){
        dialog->setCBDatasetName(DSList);
        for(int i=0; i<DSList.size(); i++){
            if(DSList[i] == windowTitle().split(".").at(0)){
                dialog->setDataDisplayedName(DSList[i]);
                dialog->setCurrentCBDSName(i);
            }
        }
    }else{
        //DSList.append(windowTitle().split(".").at(0));
        //dialog->setDataDisplayedName(DSList[0]);
    }
    dialog->show();
}

void MainWindow::aggregate()
{
    saveToR(); //Cek ada perubahan apa gak, kalo ada tawari save

    QStringList DSList = activeDatasetClass->getDatasetNameList();
    DMAggregateDialog *dialog = new DMAggregateDialog(this,R);
    dialog->setDE(dataEditor);
    dialog->setActiveDS(activeDatasetClass);
    if(DSList.size()>0){
        dialog->setCBDatasetName(DSList);
        for(int i=0; i<DSList.size(); i++){
            if(DSList[i] == windowTitle().split(".").at(0)){
                dialog->setDataDisplayedName(DSList[i]);
                dialog->setCurrentCBDSName(i);
            }
        }
    }else{
        //DSList.append(windowTitle().split(".").at(0));
        //dialog->setDataDisplayedName(DSList[0]);
    }
    dialog->show();
}

void MainWindow::mergeByCol()
{
    saveToR();

    QStringList DSList = activeDatasetClass->getDatasetNameList();
    DM_MergeByColDialog *dialog = new DM_MergeByColDialog(this,R);
    dialog->setDE(dataEditor);
    dialog->setActiveDS(activeDatasetClass);
    if(DSList.size()>0){
        dialog->setCBDatasetName(DSList);
        for(int i=0; i<DSList.size(); i++){
            if(DSList[i] == windowTitle().split(".").at(0)){
                dialog->setDataDisplayedName(DSList[i]);
            }
        }
    }else{
        //DSList.append(windowTitle().split(".").at(0));
        //dialog->setDataDisplayedName(DSList[0]);
    }
    dialog->show();
}
