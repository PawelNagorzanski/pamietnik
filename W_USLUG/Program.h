#ifndef PROGRAM_H
#define PROGRAM_H

#include <QObject>
#include <QWidget>
#include <QFont>
#include <QColor>
#include "W_DANYCH/Repozytorium.h"

struct WidokNawigacji {
    const Wpis* poprzedni = nullptr;
    const Wpis* aktualny = nullptr;
    const Wpis* nastepny = nullptr;

    bool maPoprzedni = false;
    bool maNastepny = false;
    bool moznaUsunac = false;
};

class Program : public QObject {
    Q_OBJECT
    friend class MainWindow;

public:
    explicit Program(QObject *parent = nullptr);
    ~Program();

    void start();

    void zlecenieDodajWpis(QWidget* parent);
    void zlecenieUsunWpis(int indeks);
    void zlecenieSzukajWpis(QWidget* parent);
    void zlecenieOpcje(QWidget* parent, const QFont& f, const QColor& tlo, const QColor& czcionka);

    void zlecenieBackup(QWidget* parent);
    void zleceniePrzywrocenie(QWidget* parent);

    void przejdzDoPoprzedniego();
    void przejdzDoNastepnego();
    void ustawIndeks(int indeks);

    int pobierzAktualnyIndeks() const { return m_aktualnyIndeks; }
    int pobierzLiczbeWpisow() const;
    bool czyIstniejeWpis(int indeks) const;
    const Wpis& pobierzWpis(int indeks) const;
    WidokNawigacji pobierzWidokNawigacji() const;

    void ustawFiltrEmocji(bool smutne, bool neutralne, bool radosne);
    void ustawSortowanie(bool odNajnowszych);

    static QString emocjaDoTekstu(Emocja e);
    static Emocja tekstDoEmocji(const QString& txt);

signals:
    void signalOdswiezWidok();
    void signalStatus(QString komunikat);
    void signalPrzejdzDoIndeksu(int indeks);
    void signalZmienionoWyglad(const QFont& font, const QColor& tlo, const QColor& czcionka);

private:
    Repozytorium* m_repo;
    QString m_sciezkaTxt;
    int m_aktualnyIndeks;
    KryteriaWyswietlania m_kryteria;

    void aktualizujListe();
    void sprawdzGranice();
    void zapiszZmiany();
    void ustawNaWpis(const Wpis& w);
};

#endif // PROGRAM_H
