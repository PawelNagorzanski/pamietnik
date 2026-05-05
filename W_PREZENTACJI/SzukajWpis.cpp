#include "SzukajWpis.h"
#include "ui_SzukajWpis.h"
#include <QMessageBox>
#include <cmath>

SzukajWpis::SzukajWpis(const QList<Wpis> &listaWpisow, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SzukajWpis),
    m_lista(listaWpisow)
{
    ui->setupUi(this);

    ui->timeEdit->setTime(QTime::currentTime());
    ui->radioWedlugDaty->setChecked(true);
    on_radioWedlugDaty_toggled(true);
}

SzukajWpis::~SzukajWpis()
{
    delete ui;
}

void SzukajWpis::on_radioWedlugDaty_toggled(bool checked)
{
    ui->calendar->setEnabled(checked);
    ui->timeEdit->setEnabled(checked);
    ui->textFraza->setEnabled(!checked);
}

void SzukajWpis::on_btnSzukaj_clicked()
{
    if (m_lista.isEmpty())
    {
        QMessageBox::warning(this, "Błąd", "Lista wpisów jest pusta.");
        return;
    }

    int znalezionyIndeks = -1;

    if (ui->radioWedlugDaty->isChecked()) // Szukanie po dacie
    {
        QDate wybranaData = ui->calendar->selectedDate();
        QTime wybranaGodzina = ui->timeEdit->time();

        QDateTime szukanyCzas(wybranaData, wybranaGodzina);

        int minRoznica = -1;

        for (int i = 0; i < m_lista.size(); ++i)
        {
            QDateTime czasWpisu = m_lista[i].getDataWpisu();

            int roznica = std::abs(szukanyCzas.secsTo(czasWpisu)); // Liczę różnicę w sekundach

            if (minRoznica == -1 || roznica < minRoznica)
            {
                minRoznica = roznica;
                znalezionyIndeks = i;
            }
        }
    }
    else if (ui->radioWedlugFrazy->isChecked()) // Szukanie po tekście
    {
        QString szukanaFraza = ui->textFraza->toPlainText();

        if (szukanaFraza.isEmpty())
        {
            QMessageBox::information(this, "Info", "Wpisz tekst do wyszukania.");
            return;
        }

        for (int i = 0; i < m_lista.size(); ++i)
        {
            if (m_lista[i].getTresc().contains(szukanaFraza, Qt::CaseInsensitive))
            {
                znalezionyIndeks = i;
                break;
            }
        }
    }

    if (znalezionyIndeks != -1) // Wynik
    {
        emit signalZnaleziono(znalezionyIndeks);
        this->accept();
    }
    else
    {
        QMessageBox::information(this, "Wynik", "Nie znaleziono pasującego wpisu.");
    }
}

void SzukajWpis::on_btnAnuluj_clicked()
{
    this->reject();
}
