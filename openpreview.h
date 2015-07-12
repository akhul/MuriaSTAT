#ifndef OPENPREVIEW_H
#define OPENPREVIEW_H

#include <QDialog>

namespace Ui {
class OpenPreview;
}

class OpenPreview : public QDialog
{
    Q_OBJECT

public:
    explicit OpenPreview(QWidget *parent = 0);
    ~OpenPreview();
    void setInputfile(QString data);
    void setDatasetName(QString name);
    void setMissing(QString missing);
    void setSeparator(QChar sep);
    void setDecimal(QString dec);
    void setComma(QString comma);
    void setQuote(QString quote);
    void setHeading(QString header);
    QString getDatasetName();
    QString getMissing();
    QString getDecimal();
    QString getQuote();
    QChar getSeparator();
    QString getHeading();
    QString getComma();


private:
    Ui::OpenPreview *ui;
    QString header;
    QString comma;
    QString datasetName;
    QChar separator;
    QString quote;
    QString missing;
    QString decimal;

};

#endif // OPENPREVIEW_H
