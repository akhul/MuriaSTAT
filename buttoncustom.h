#ifndef BUTTONCUSTOM_H
#define BUTTONCUSTOM_H

#include <QWidget>
#include <QPushButton>

class ButtonCustom : public QPushButton
{
    Q_OBJECT
public:
    explicit ButtonCustom(QWidget *parent = 0);
    ~ButtonCustom();
    void setCollapseIcon();

private:
    QIcon expandStatus;
    QIcon collapseStatus;

signals:

public slots:
    void setNewIcon();
    void setExpandIcon();
};

#endif // BUTTONCUSTOM_H
