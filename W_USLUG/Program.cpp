#include "Program.h"
#include "W_PREZENTACJI/DodajWpis.h"
#include "W_PREZENTACJI/SzukajWpis.h"
#include "W_PREZENTACJI/Opcje.h"
#include <QCoreApplication>
#include <QFileDialog>
#include <QDebug>

Program::Program(QObject *parent)
    : QObject(parent)
    , m_aktualnyIndeks(0)
{
    m_repo = new Repozytorium();
    m_sciezkaTxt = QCoreApplication::applicationDirPath() + "/save_pamietnika.txt";

    m_kryteria.pokazSmutne = true;
    m_kryteria.pokazNeutralne = true;
    m_kryteria.pokazRadosne = true;
    m_kryteria.sortujOdNajnowszych = true;
}

Program::~Program() {
    delete m_repo;
}

void Program::start() {
    bool sukces = m_repo->wczytajZBazy(m_aktualnyIndeks);

    if (!sukces) {
        sukces = m_repo->wczytajZPliku(m_sciezkaTxt, m_aktualnyIndeks);
        if (sukces) {
            emit signalStatus("Wczytano ze starego pliku tekstowego.");
            zapiszZmiany();
        } else {
            emit signalStatus("Utworzono nowy dziennik.");
        }
    } else {
        emit signalStatus("Wczytano z bazy danych.");
    }

    aktualizujListe();
    emit signalOdswiezWidok();
}

void Program::zapiszZmiany() {
    m_repo->zapiszDoBazy(m_aktualnyIndeks);
    m_repo->zapiszDoPliku(m_sciezkaTxt, m_aktualnyIndeks);
}

void Program::ustawNaWpis(const Wpis& w) {
    int ilosc = m_repo->liczbaWpisow();
    for(int i = 0; i < ilosc; ++i) {
        const Wpis* kandydat = m_repo->pobierzWpis(i);
        if (kandydat && *kandydat == w) {
            m_aktualnyIndeks = i;
            break;
        }
    }
}

void Program::zlecenieDodajWpis(QWidget* parent) {
    DodajWpis okno(parent);
    connect(&okno, &DodajWpis::signalWpis, this, [this](const Wpis& wpis) {
        bool zaktualizowano = m_repo->dodajLubAktualizuj(wpis);
        zapiszZmiany();
        aktualizujListe();
        ustawNaWpis(wpis);

        if(zaktualizowano) emit signalStatus("Zaktualizowano wpis.");
        else emit signalStatus("Dodano nowy wpis.");
        emit signalOdswiezWidok();
    });
    okno.exec();
}

void Program::zlecenieUsunWpis(int indeks) {
    if (!czyIstniejeWpis(indeks)) {
        emit signalStatus("Brak wpisu do usunięcia.");
        return;
    }
    const Wpis* ptr = m_repo->pobierzWpis(indeks);
    if (!ptr) return;
    Wpis doUsuniecia = *ptr;

    if (m_repo->usunWpis(doUsuniecia)) {
        zapiszZmiany();
        aktualizujListe();
        sprawdzGranice();
        emit signalOdswiezWidok();
        emit signalStatus("Usunięto wpis.");
    }
}

void Program::zlecenieSzukajWpis(QWidget* parent) {
    QList<Wpis> listaKopii;
    int ilosc = m_repo->liczbaWpisow();
    for(int i=0; i<ilosc; i++) {
        const Wpis* w = m_repo->pobierzWpis(i);
        if(w) listaKopii.append(*w);
    }

    SzukajWpis okno(listaKopii, parent);
    connect(&okno, &SzukajWpis::signalZnaleziono, this, [this](int indeks) {
        ustawIndeks(indeks);
        emit signalStatus("Przeskoczono do znalezionego wpisu.");
    });
    okno.exec();
}

void Program::zlecenieOpcje(QWidget* parent, const QFont& f, const QColor& tlo, const QColor& czcionka) {
    Opcje okno(parent);

    okno.setSortujOdNajnowszych(m_kryteria.sortujOdNajnowszych);
    okno.setAktualnaCzcionka(f);
    okno.setAktualnyKolorTla(tlo);
    okno.setAktualnyKolorCzcionki(czcionka);

    connect(&okno, &Opcje::signalZapiszUstawienia, this,
            [this](const QFont& font, const QColor& tlo, const QColor& czcionka, bool sortowanie) {
                bool zmiana = (m_kryteria.sortujOdNajnowszych != sortowanie);
                m_kryteria.sortujOdNajnowszych = sortowanie;
                if(zmiana) {
                    aktualizujListe();
                    m_aktualnyIndeks = 0;
                }
                emit signalZmienionoWyglad(font, tlo, czcionka);
                emit signalOdswiezWidok();
                emit signalStatus("Ustawienia zastosowane.");
            });

    connect(&okno, &Opcje::signalZapiszKopieZapasowa, this, [this, parent]() {
        zlecenieBackup(parent);
    });

    connect(&okno, &Opcje::signalPrzywrocZKopii, this, [this, parent]() {
        zleceniePrzywrocenie(parent);
    });

    okno.exec();
}

void Program::zlecenieBackup(QWidget* parent) {
    QString filtry = "Pliki tekstowe (*.txt);;Baza danych SQLite (*.sqlite *.db)";
    QString wybranyFiltr;

    QString nazwaPliku = QFileDialog::getSaveFileName(parent,
                                                      "Zapisz kopię zapasową", "", filtry, &wybranyFiltr);

    if (!nazwaPliku.isEmpty()) {
        bool wynik = false;

        if (nazwaPliku.endsWith(".sqlite") || nazwaPliku.endsWith(".db") || wybranyFiltr.contains("SQLite")) {
            wynik = m_repo->zapiszJakoBaze(nazwaPliku, m_aktualnyIndeks);
        } else {
            if (!nazwaPliku.contains(".")) nazwaPliku += ".txt";
            wynik = m_repo->zapiszDoPliku(nazwaPliku, m_aktualnyIndeks);
        }

        if (wynik) emit signalStatus("Zapisano kopię zapasową.");
        else emit signalStatus("Błąd zapisu kopii!");
    }
}

void Program::zleceniePrzywrocenie(QWidget* parent) {
    QString filtry = "Wszystkie obsługiwane (*.txt *.sqlite *.db);;Pliki tekstowe (*.txt);;Baza danych SQLite (*.sqlite *.db)";

    QString nazwaPliku = QFileDialog::getOpenFileName(parent,
                                                      "Wczytaj z pliku", "", filtry);

    if (!nazwaPliku.isEmpty()) {
        int nowyIndeks = 0;
        bool sukces = false;

        if (nazwaPliku.endsWith(".sqlite") || nazwaPliku.endsWith(".db")) {
            sukces = m_repo->wczytajZPlikuBazy(nazwaPliku, nowyIndeks);
        } else {
            sukces = m_repo->wczytajZPliku(nazwaPliku, nowyIndeks);
        }

        if (sukces) {
            m_aktualnyIndeks = nowyIndeks;
            zapiszZmiany();

            aktualizujListe();
            emit signalOdswiezWidok();
            emit signalStatus("Przywrócono dane z pliku.");
        } else {
            emit signalStatus("Błąd odczytu pliku!");
        }
    }
}

void Program::przejdzDoPoprzedniego() {
    if(m_aktualnyIndeks > 0) {
        m_aktualnyIndeks--;
        zapiszZmiany();
        emit signalOdswiezWidok();
    }
}

void Program::przejdzDoNastepnego() {
    if(m_aktualnyIndeks < m_repo->liczbaWpisow() - 1) {
        m_aktualnyIndeks++;
        zapiszZmiany();
        emit signalOdswiezWidok();
    }
}

void Program::ustawIndeks(int indeks) {
    if(czyIstniejeWpis(indeks)) {
        m_aktualnyIndeks = indeks;
        emit signalPrzejdzDoIndeksu(indeks);
        emit signalOdswiezWidok();
    }
}

void Program::ustawFiltrEmocji(bool smutne, bool neutralne, bool radosne) {
    m_kryteria.pokazSmutne = smutne;
    m_kryteria.pokazNeutralne = neutralne;
    m_kryteria.pokazRadosne = radosne;
    aktualizujListe();
    sprawdzGranice();
    emit signalOdswiezWidok();
}

void Program::ustawSortowanie(bool odNajnowszych) {
    if(m_kryteria.sortujOdNajnowszych != odNajnowszych) {
        m_kryteria.sortujOdNajnowszych = odNajnowszych;
        aktualizujListe();
        m_aktualnyIndeks = 0;
        emit signalOdswiezWidok();
    }
}

int Program::pobierzLiczbeWpisow() const {
    return m_repo->liczbaWpisow();
}

bool Program::czyIstniejeWpis(int indeks) const {
    return indeks >= 0 && indeks < m_repo->liczbaWpisow();
}

const Wpis& Program::pobierzWpis(int indeks) const {
    const Wpis* w = m_repo->pobierzWpis(indeks);
    if (w) return *w;
    static Wpis pusty;
    return pusty;
}

WidokNawigacji Program::pobierzWidokNawigacji() const {
    WidokNawigacji widok;
    int indeks = m_aktualnyIndeks;
    int ilosc = m_repo->liczbaWpisow();

    widok.poprzedni = m_repo->pobierzWpis(indeks - 1);
    widok.aktualny  = m_repo->pobierzWpis(indeks);
    widok.nastepny  = m_repo->pobierzWpis(indeks + 1);

    widok.maPoprzedni = (indeks > 0);
    widok.maNastepny  = (indeks < ilosc - 1);
    widok.moznaUsunac = (ilosc > 0);

    return widok;
}

QString Program::emocjaDoTekstu(Emocja e) {
    switch(e) {
    case Emocja::Smutne: return "Smutne :(";
    case Emocja::Neutralne: return "Neutralne :/";
    case Emocja::Radosne: return "Radosne :)";
    }
    return "Nieznane";
}

Emocja Program::tekstDoEmocji(const QString& txt) {
    if(txt == "Smutne :(") return Emocja::Smutne;
    if(txt == "Radosne :)") return Emocja::Radosne;
    return Emocja::Neutralne;
}

void Program::aktualizujListe() {
    m_repo->przeliczWidok(m_kryteria);
}

void Program::sprawdzGranice() {
    int ilosc = m_repo->liczbaWpisow();
    if(ilosc == 0) {
        m_aktualnyIndeks = 0;
        return;
    }
    if(m_aktualnyIndeks >= ilosc) {
        m_aktualnyIndeks = ilosc - 1;
    }
    if (m_aktualnyIndeks < 0) {
        m_aktualnyIndeks = 0;
    }
}
