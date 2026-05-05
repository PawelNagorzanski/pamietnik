#include "DodajWpis.h"
#include "W_DANYCH/Wpis.h"
#include "ui_DodajWpis.h"
#include <QButtonGroup>

DodajWpis::DodajWpis(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DodajWpis)
{
    ui->setupUi(this);
    ui->dataWpisu->setDateTime(QDateTime::currentDateTime());

    QButtonGroup *group = new QButtonGroup(this);
    group->addButton(ui->checkBoxRadosne);
    group->addButton(ui->checkBoxNeutralne);
    group->addButton(ui->checkBoxSmutne);
    group->setExclusive(true);
    ui->checkBoxNeutralne->setChecked(true);
}

DodajWpis::~DodajWpis()
{
    delete ui;
}

void DodajWpis::on_dodajWpis_clicked()
{
    QDateTime dataCzas = ui->dataWpisu->dateTime();
    QString tresc = ui->textEdycja->toPlainText();
    Emocja emocja = Emocja::Neutralne;
    if (ui->checkBoxRadosne->isChecked()) emocja = Emocja::Radosne;
    else if (ui->checkBoxNeutralne->isChecked()) emocja = Emocja::Neutralne;
    else emocja = Emocja::Smutne;
    Wpis nowyWpis(dataCzas, tresc, emocja);
    emit signalWpis(nowyWpis);
    this->accept();
}

void DodajWpis::on_btnAnuluj_clicked()
{
    ui->textEdycja->clear();
    this->reject();
}

