#ifndef REPOZYTORIUM_H
#define REPOZYTORIUM_H

#include "Wpis.h"
#include <QList>
#include <QString>
#include <QSqlDatabase>

struct KryteriaWyswietlania {
    bool pokazSmutne = true;
    bool pokazNeutralne = true;
    bool pokazRadosne = true;
    bool sortujOdNajnowszych = true;
};

class Repozytorium
{
    friend class MainWindow;

public:
    Repozytorium();
    ~Repozytorium();

    bool polaczZBaza();
    bool wczytajZBazy(int& outIndeks);
    bool zapiszDoBazy(int aktualnyIndeks);

    bool zapiszJakoBaze(const QString& sciezka, int aktualnyIndeks);
    bool wczytajZPlikuBazy(const QString& sciezka, int& outIndeks);

    bool wczytajZPliku(const QString& sciezka, int& outIndeks);
    bool zapiszDoPliku(const QString& sciezka, int aktualnyIndeks);

    void przeliczWidok(const KryteriaWyswietlania& kryteria);
    const Wpis* pobierzWpis(int indeks) const;
    int liczbaWpisow() const;

    bool dodajLubAktualizuj(const Wpis& nowyWpis);
    bool usunWpis(const Wpis& doUsuniecia);

private:
    QList<Wpis*> m_wszystkie;
    QList<Wpis*> m_widoczne;

    QSqlDatabase m_db;
};

#endif // REPOZYTORIUM_H
