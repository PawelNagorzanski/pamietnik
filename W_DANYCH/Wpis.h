#ifndef WPIS_H
#define WPIS_H

#include <QDateTime>
#include <QString>
#include <QTextStream>

enum class Emocja {
    Smutne = 0,
    Neutralne = 1,
    Radosne = 2
};

class Wpis
{
private:
    QDateTime m_dataWpisu;
    QString m_tresc;
    Emocja m_emocja;

public:
    Wpis();
    Wpis(QDateTime data, QString tresc, Emocja emocja);

    void setDataWpisu(QDateTime i_dataWpisu);
    void setTresc(QString i_tresc);
    void setEmocja(Emocja e);

    QDateTime getDataWpisu() const;
    QString getTresc() const;
    Emocja getEmocja() const;

    void zapiszDoStrumienia(QTextStream& out) const;

    static Wpis wczytajZeStrumienia(QTextStream& in);

    bool jestDuplikatem(const Wpis& inny) const;

    bool operator==(const Wpis& inny) const;
    bool operator<(const Wpis& inny) const;
    bool operator>(const Wpis& inny) const;
};

#endif // WPIS_H
