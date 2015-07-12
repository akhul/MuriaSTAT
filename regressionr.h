#ifndef REGRESSIONR_H
#define REGRESSIONR_H

#include <RInside.h>
#include <QtGui>


class RegressionR
{
public:
    RegressionR(RInside &m_R);
    ~RegressionR();
    QString getAllHtml();
    void setcoefHtml();
    void setStatIndicator(QStringList stat);
    void setOuputHtml(std::string model,std::string datasetName);
    void setAnova();
    void setAllHtml();
    void setSummaryModel();
    void setResidualHtml();


private:
    RInside &R;
    QString coefHtml;
    QString anovaHtml;
    QString resHtml;
    QString summaryModelHtml;
    QStringList StatIndicator;
    QString allHtml;


};

#endif // REGRESSIONR_H
