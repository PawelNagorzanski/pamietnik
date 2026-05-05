#include "Opcje.h"
#include "ui_Opcje.h"
#include <QColorDialog>

Opcje::Opcje(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Opcje)
{
    ui->setupUi(this);

    m_kolorTla = Qt::white;
    m_kolorCzcionki = Qt::black;

    aktualizujPodglad();
}

Opcje::~Opcje()
{
    delete ui;
}

void Opcje::on_btnZastosuj_clicked()
{
    QFont font = ui->fontComboBox->currentFont();
    font.setPointSize(ui->spinRozmiar->value());
    bool sortowanieMalejaco = ui->radioNajnowsze->isChecked();
    emit signalZapiszUstawienia(font, m_kolorTla, m_kolorCzcionki, sortowanieMalejaco);
    this->accept();
}

void Opcje::on_btnAnuluj_clicked()
{
    this->reject();
}

void Opcje::on_btnWybierzKolor_clicked()
{
    QColor kolor = QColorDialog::getColor(m_kolorTla, this, "Wybierz kolor tła");
    if (kolor.isValid()) {
        m_kolorTla = kolor;
        aktualizujPodglad();
    }
}

void Opcje::on_btnWybierzKolorCzcionki_clicked()
{
    QColor kolor = QColorDialog::getColor(m_kolorCzcionki, this, "Wybierz kolor czcionki");
    if (kolor.isValid()) {
        m_kolorCzcionki = kolor;
        aktualizujPodglad();
    }
}

void Opcje::aktualizujPodglad()
{
    if(ui->previewKolor)
        ui->previewKolor->setStyleSheet(QString("background-color: %1; border: 1px solid gray;").arg(m_kolorTla.name()));

    if(ui->previewKolorCzcionki)
        ui->previewKolorCzcionki->setStyleSheet(QString("background-color: %1; border: 1px solid gray;").arg(m_kolorCzcionki.name()));
}

void Opcje::setAktualnaCzcionka(const QFont &font)
{
    ui->fontComboBox->setCurrentFont(font);
    ui->spinRozmiar->setValue(font.pointSize());
}

void Opcje::setAktualnyKolorTla(const QColor &color)
{
    m_kolorTla = color;
    aktualizujPodglad();
}

void Opcje::setAktualnyKolorCzcionki(const QColor &color)
{
    m_kolorCzcionki = color;
    aktualizujPodglad();
}

void Opcje::setSortujOdNajnowszych(bool najnowsze)
{
    if (najnowsze) ui->radioNajnowsze->setChecked(true);
    else ui->radioNajstarsze->setChecked(true);
}

void Opcje::on_pushZapiszKopie_clicked()
{
    emit signalZapiszKopieZapasowa();
}

void Opcje::on_pushWczytajKopie_clicked()
{
    emit signalPrzywrocZKopii();
}
