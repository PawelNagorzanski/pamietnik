#include "Wpis.h"

Wpis::Wpis() : m_emocja(Emocja::Neutralne) {}

Wpis::Wpis(QDateTime data, QString tresc, Emocja emocja)
    : m_dataWpisu(data), m_tresc(tresc), m_emocja(emocja) {}

QDateTime Wpis::getDataWpisu() const { return m_dataWpisu; }
QString Wpis::getTresc() const { return m_tresc; }
Emocja Wpis::getEmocja() const { return m_emocja; }
void Wpis::setTresc(QString i_tresc) { m_tresc = i_tresc; }
void Wpis::setEmocja(Emocja e) { m_emocja = e; }
void Wpis::setDataWpisu(QDateTime d) { m_dataWpisu = d; }

void Wpis::zapiszDoStrumienia(QTextStream& out) const {
    out << m_dataWpisu.toString(Qt::ISODate) << "\n";
    out << static_cast<int>(m_emocja) << "\n";
    out << m_tresc.length() << "\n";
    out << m_tresc << "\n";
}

Wpis Wpis::wczytajZeStrumienia(QTextStream& in) {
    if (in.atEnd()) return Wpis();

    QString dataStr = in.readLine();
    if (dataStr.isNull()) return Wpis();
    QDateTime data = QDateTime::fromString(dataStr, Qt::ISODate);

    int emocjaInt = in.readLine().toInt();
    Emocja emocja = static_cast<Emocja>(emocjaInt);

    int dlugosc = in.readLine().toInt();
    QString tresc = "";
    if (dlugosc > 0) {
        tresc = in.read(dlugosc);
        in.readLine();
    } else {
        in.readLine();
    }

    if (data.isValid()) {
        return Wpis(data, tresc, emocja);
    }
    return Wpis();
}

bool Wpis::jestDuplikatem(const Wpis& inny) const {
    QString format = "dd.MM.yyyy hh:mm";
    return m_dataWpisu.toString(format) == inny.m_dataWpisu.toString(format);
}

bool Wpis::operator==(const Wpis& inny) const {
    return jestDuplikatem(inny);
}

bool Wpis::operator<(const Wpis& inny) const {
    return m_dataWpisu < inny.m_dataWpisu;
}
bool Wpis::operator>(const Wpis& inny) const {
    return m_dataWpisu > inny.m_dataWpisu;
}
