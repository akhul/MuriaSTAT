#include "buttoncustom.h"

ButtonCustom::ButtonCustom(QWidget *parent) : QPushButton(parent)
{
    expandStatus.addFile(QString::fromUtf8(":/image/stop (2).png"), QSize(), QIcon::Normal, QIcon::Off);
    collapseStatus.addFile(QString::fromUtf8(":/image/run.png"), QSize(), QIcon::Normal, QIcon::Off);

    expandStatus.setThemeName("expand");
    collapseStatus.setThemeName("collapse");
    connect(this,SIGNAL(clicked()),this,SLOT(setNewIcon()));
}

ButtonCustom::~ButtonCustom()
{

}
void ButtonCustom::setExpandIcon(){
    setIcon(expandStatus);

}
void ButtonCustom::setCollapseIcon(){
    setIcon(collapseStatus);

}


void ButtonCustom::setNewIcon(){

    if(icon().themeName()==QString("expand")){
        collapseStatus.setThemeName("collapse");
        setIcon(collapseStatus);

    }
    else{
        expandStatus.setThemeName("expand");
        setIcon(expandStatus);


    }
}

