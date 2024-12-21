#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_clientapp.h"

class clientapp : public QMainWindow
{
    Q_OBJECT

public:
    clientapp(QWidget *parent = nullptr);
    ~clientapp();

private:
    Ui::clientappClass ui;
};
