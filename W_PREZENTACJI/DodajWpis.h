#ifndef DODAJWPIS_H
#define DODAJWPIS_H


#include <QDialog>
#include "W_DANYCH/Wpis.h"

namespace Ui {
class DodajWpis;
}

class DodajWpis : public QDialog
{
    Q_OBJECT

public:
    explicit DodajWpis(QWidget *parent = nullptr);
    ~DodajWpis();
    //Wpis getUtworzonyWpis() const;
signals:
    void signalWpis(const Wpis& i_nowyWpis);
private:
    Ui::DodajWpis *ui;
private slots:
    void on_dodajWpis_clicked();
    void on_btnAnuluj_clicked();
};

#endif // DODAJWPIS_H
