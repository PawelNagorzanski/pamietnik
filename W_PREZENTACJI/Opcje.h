#ifndef OPCJE_H
#define OPCJE_H

#include <QDialog>
#include <QColor>
#include <QFont>

namespace Ui {
class Opcje;
}

class Opcje : public QDialog
{
    Q_OBJECT

public:
    explicit Opcje(QWidget *parent = nullptr);
    ~Opcje();

    void setAktualnaCzcionka(const QFont &font);
    void setAktualnyKolorTla(const QColor &color);
    void setAktualnyKolorCzcionki(const QColor &color);
    void setSortujOdNajnowszych(bool najnowsze);

signals:
    void signalZapiszUstawienia(const QFont& font, const QColor& tlo, const QColor& czcionka, bool sortowanie);
    void signalZapiszKopieZapasowa();
    void signalPrzywrocZKopii();

private slots:
    void on_btnWybierzKolor_clicked();
    void on_btnWybierzKolorCzcionki_clicked();

    void on_btnZastosuj_clicked();
    void on_btnAnuluj_clicked();

    void on_pushZapiszKopie_clicked();
    void on_pushWczytajKopie_clicked();

private:
    Ui::Opcje *ui;
    QColor m_kolorTla;
    QColor m_kolorCzcionki;

    void aktualizujPodglad();
};

#endif // OPCJE_H
