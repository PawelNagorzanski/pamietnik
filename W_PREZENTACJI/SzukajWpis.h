#ifndef SZUKAJWPIS_H
#define SZUKAJWPIS_H

#include <QDialog>
#include "W_DANYCH/Wpis.h"

namespace Ui {
class SzukajWpis;
}

class SzukajWpis : public QDialog
{
    Q_OBJECT

public:
    explicit SzukajWpis(const QList<Wpis> &listaWpisow, QWidget *parent = nullptr);
    ~SzukajWpis();

signals:
    void signalZnaleziono(int indeks);

private slots:
    void on_btnSzukaj_clicked();
    void on_btnAnuluj_clicked();

    void on_radioWedlugDaty_toggled(bool checked);

private:
    Ui::SzukajWpis *ui;
    const QList<Wpis> &m_lista;
};

#endif // SZUKAJWPIS_H
