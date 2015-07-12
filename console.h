#ifndef CONSOLE_H
#define CONSOLE_H

#include <QWidget>
#include <QPushButton>

class Console : public QWidget
{
    Q_OBJECT
public:
    explicit Console(QWidget *parent = 0);
    ~Console();

signals:

public slots:
};

#endif // CONSOLE_H
