#include "Repozytorium.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QUuid>
#include <QDebug>
#include <algorithm>

Repozytorium::Repozytorium()
{
    polaczZBaza();
}

Repozytorium::~Repozytorium() {
    qDeleteAll(m_wszystkie);
    if(m_db.isOpen()) m_db.close();
}

bool Repozytorium::polaczZBaza() {
    if (m_db.isValid() && m_db.isOpen()) return true;

    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(QCoreApplication::applicationDirPath() + "/pamietnik.sqlite");

    if (!m_db.open()) return false;

    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS wpisy ("
               "data_wpisu TEXT PRIMARY KEY, "
               "emocja INTEGER, "
               "tresc TEXT, "
               "ostatni_indeks INTEGER)");
    return true;
}

bool Repozytorium::zapiszDoBazy(int aktualnyIndeks) {
    if (!m_db.isOpen() && !polaczZBaza()) return false;

    m_db.transaction();
    QSqlQuery query;
    query.exec("DELETE FROM wpisy");

    query.prepare("INSERT INTO wpisy (data_wpisu, emocja, tresc, ostatni_indeks) VALUES (:d, :e, :t, :i)");

    for (Wpis* w : m_wszystkie) {
        query.bindValue(":d", w->getDataWpisu().toString(Qt::ISODate));
        query.bindValue(":e", static_cast<int>(w->getEmocja()));
        query.bindValue(":t", w->getTresc());
        query.bindValue(":i", aktualnyIndeks);
        query.exec();
    }
    return m_db.commit();
}

bool Repozytorium::wczytajZBazy(int& outIndeks) {
    if (!m_db.isOpen() && !polaczZBaza()) return false;

    qDeleteAll(m_wszystkie);
    m_wszystkie.clear();
    m_widoczne.clear();

    QSqlQuery query("SELECT data_wpisu, emocja, tresc, ostatni_indeks FROM wpisy");

    bool czySaDane = false;
    bool pierwszy = true;

    while (query.next()) {
        czySaDane = true;
        QString d = query.value(0).toString();
        int e = query.value(1).toInt();
        QString t = query.value(2).toString();

        if (pierwszy) {
            outIndeks = query.value(3).toInt();
            pierwszy = false;
        }

        QDateTime dt = QDateTime::fromString(d, Qt::ISODate);
        if (dt.isValid()) {
            m_wszystkie.append(new Wpis(dt, t, static_cast<Emocja>(e)));
        }
    }
    return czySaDane;
}

bool Repozytorium::zapiszJakoBaze(const QString& sciezka, int aktualnyIndeks) {
    QString nazwa = "backup_" + QUuid::createUuid().toString();
    bool sukces = false;

    {
        QSqlDatabase dbBackup = QSqlDatabase::addDatabase("QSQLITE", nazwa);
        dbBackup.setDatabaseName(sciezka);

        if (dbBackup.open()) {
            QSqlQuery q(dbBackup);
            q.exec("CREATE TABLE IF NOT EXISTS wpisy (data_wpisu TEXT PRIMARY KEY, emocja INTEGER, tresc TEXT, ostatni_indeks INTEGER)");
            q.exec("DELETE FROM wpisy");

            dbBackup.transaction();
            q.prepare("INSERT INTO wpisy (data_wpisu, emocja, tresc, ostatni_indeks) VALUES (:d, :e, :t, :i)");

            for (Wpis* w : m_wszystkie) {
                q.bindValue(":d", w->getDataWpisu().toString(Qt::ISODate));
                q.bindValue(":e", static_cast<int>(w->getEmocja()));
                q.bindValue(":t", w->getTresc());
                q.bindValue(":i", aktualnyIndeks);
                q.exec();
            }
            sukces = dbBackup.commit();
            dbBackup.close();
        }
    }
    QSqlDatabase::removeDatabase(nazwa); // Sprzątamy połączenie
    return sukces;
}

bool Repozytorium::wczytajZPlikuBazy(const QString& sciezka, int& outIndeks) {
    QString nazwa = "restore_" + QUuid::createUuid().toString();
    bool sukces = false;

    {
        QSqlDatabase dbRestore = QSqlDatabase::addDatabase("QSQLITE", nazwa);
        dbRestore.setDatabaseName(sciezka);

        if (dbRestore.open()) {
            qDeleteAll(m_wszystkie);
            m_wszystkie.clear();
            m_widoczne.clear();

            QSqlQuery q(dbRestore);
            q.exec("SELECT data_wpisu, emocja, tresc, ostatni_indeks FROM wpisy");

            bool pierwszy = true;
            while (q.next()) {
                QString d = q.value(0).toString();
                int e = q.value(1).toInt();
                QString t = q.value(2).toString();

                if (pierwszy) { outIndeks = q.value(3).toInt(); pierwszy = false; }

                QDateTime dt = QDateTime::fromString(d, Qt::ISODate);
                if (dt.isValid()) m_wszystkie.append(new Wpis(dt, t, static_cast<Emocja>(e)));
            }
            sukces = !m_wszystkie.isEmpty();
            dbRestore.close();
        }
    }
    QSqlDatabase::removeDatabase(nazwa);
    return sukces;
}

bool Repozytorium::zapiszDoPliku(const QString& sciezka, int aktualnyIndeks) {
    QFile plik(sciezka);
    if (!plik.open(QIODevice::WriteOnly | QIODevice::Text)) return false;

    QTextStream out(&plik);

    out << aktualnyIndeks << "\n";
    for (auto w : m_wszystkie) w->zapiszDoStrumienia(out);
    return true;
}

bool Repozytorium::wczytajZPliku(const QString& sciezka, int& outIndeks) {
    QFile plik(sciezka);
    if (!plik.exists() || !plik.open(QIODevice::ReadOnly | QIODevice::Text)) return false;

    qDeleteAll(m_wszystkie);
    m_wszystkie.clear();
    m_widoczne.clear();

    QTextStream in(&plik);

    QString l = in.readLine();
    if (!l.isNull()) outIndeks = l.toInt();

    while (!in.atEnd()) {
        Wpis t = Wpis::wczytajZeStrumienia(in);
        if (t.getDataWpisu().isValid()) m_wszystkie.append(new Wpis(t));
    }
    return true;
}

void Repozytorium::przeliczWidok(const KryteriaWyswietlania& k) {
    m_widoczne.clear();
    for (auto w : m_wszystkie) {
        if (w->getEmocja() == Emocja::Smutne && !k.pokazSmutne) continue;
        if (w->getEmocja() == Emocja::Neutralne && !k.pokazNeutralne) continue;
        if (w->getEmocja() == Emocja::Radosne && !k.pokazRadosne) continue;
        m_widoczne.append(w);
    }
    std::sort(m_widoczne.begin(), m_widoczne.end(), [&](Wpis* a, Wpis* b){
        return k.sortujOdNajnowszych ? (*a > *b) : (*a < *b);
    });
}

const Wpis* Repozytorium::pobierzWpis(int indeks) const {
    if (indeks < 0 || indeks >= m_widoczne.size()) return nullptr;
    return m_widoczne[indeks];
}

int Repozytorium::liczbaWpisow() const { return m_widoczne.size(); }

bool Repozytorium::dodajLubAktualizuj(const Wpis& dane) {
    for (auto w : m_wszystkie) {
        if (*w == dane) {
            *w = dane;
            return true;
        }
    }
    m_wszystkie.append(new Wpis(dane));
    return false;
}

bool Repozytorium::usunWpis(const Wpis& doUsuniecia) {
    for (int i = 0; i < m_wszystkie.size(); ++i) {
        if (*m_wszystkie[i] == doUsuniecia) {
            delete m_wszystkie[i];
            m_wszystkie.removeAt(i);
            return true;
        }
    }
    return false;
}
