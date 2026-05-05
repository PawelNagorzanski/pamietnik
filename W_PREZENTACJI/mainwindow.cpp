#include "mainwindow.h"
#include "W_USLUG/Program.h"
#include "ui_mainwindow.h"
#include "W_DANYCH/Wpis.h"
#include <QKeyEvent>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent, Program* program)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_program(program)
{
    ui->setupUi(this);
    qApp->installEventFilter(this);

    ustawDomyslneStyleWizualne();
    zastosujStyle();

    connect(m_program, &Program::signalOdswiezWidok,
            this, &MainWindow::slotOdswiezWidok);

    connect(m_program, &Program::signalStatus,
            this, &MainWindow::slotPokazStatus);

    connect(m_program, &Program::signalPrzejdzDoIndeksu,
            this, &MainWindow::slotPrzejdzDoIndeksu);

    connect(m_program, &Program::signalZmienionoWyglad,
            this, &MainWindow::slotZmienionoWyglad);

    ui->checkSmutne->blockSignals(true);
    ui->checkNeutralne->blockSignals(true);
    ui->checkRadosne->blockSignals(true);

    ui->checkSmutne->setChecked(true);
    ui->checkNeutralne->setChecked(true);
    ui->checkRadosne->setChecked(true);

    ui->checkSmutne->blockSignals(false);
    ui->checkNeutralne->blockSignals(false);
    ui->checkRadosne->blockSignals(false);
}

MainWindow::~MainWindow()
{
    qApp->removeEventFilter(this);
    delete ui;
}

void MainWindow::on_btnPoprzedni_clicked() {
    m_program->przejdzDoPoprzedniego();
}

void MainWindow::on_btnNastepny_clicked() {
    m_program->przejdzDoNastepnego();
}

void MainWindow::on_btnUsun_clicked() {
    int indeks = m_program->pobierzAktualnyIndeks();
    m_program->zlecenieUsunWpis(indeks);
}

void MainWindow::on_btnDodajWpis_clicked() {
    m_program->zlecenieDodajWpis(this);
}

void MainWindow::on_btnSzukaj_clicked() {
    m_program->zlecenieSzukajWpis(this);
}

void MainWindow::on_btnOpcje_clicked() {
    m_program->zlecenieOpcje(this, m_czcionka, m_kolorTla, m_kolorCzcionki);
}

void MainWindow::on_checkSmutne_toggled(bool checked) {
    m_program->ustawFiltrEmocji(checked, ui->checkNeutralne->isChecked(), ui->checkRadosne->isChecked());
}

void MainWindow::on_checkNeutralne_toggled(bool checked) {
    m_program->ustawFiltrEmocji(ui->checkSmutne->isChecked(), checked, ui->checkRadosne->isChecked());
}

void MainWindow::on_checkRadosne_toggled(bool checked) {
    m_program->ustawFiltrEmocji(ui->checkSmutne->isChecked(), ui->checkNeutralne->isChecked(), checked);
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event) {
    if(event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if(keyEvent->modifiers() == Qt::NoModifier) {
            if(keyEvent->key() == Qt::Key_Up) {
                on_btnPoprzedni_clicked();
                return true;
            }
            if(keyEvent->key() == Qt::Key_Down) {
                on_btnNastepny_clicked();
                return true;
            }
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::slotOdswiezWidok() {
    aktualizujWyswietlanie();
}

void MainWindow::slotPokazStatus(QString komunikat) {
    ui->statusbar->showMessage(komunikat, 3000);
}

void MainWindow::slotPrzejdzDoIndeksu(int indeks) {
    aktualizujWyswietlanie();
}

void MainWindow::aktualizujWyswietlanie() {
    WidokNawigacji widok = m_program->pobierzWidokNawigacji();

    if (widok.poprzedni) wyswietlWpisWPolu(ui->textLista_pop, *widok.poprzedni);
    else wyswietlPustyWpis(ui->textLista_pop);

    if (widok.aktualny)  wyswietlWpisWPolu(ui->textLista_akt, *widok.aktualny);
    else wyswietlPustyWpis(ui->textLista_akt);

    if (widok.nastepny)  wyswietlWpisWPolu(ui->textLista_nas, *widok.nastepny);
    else wyswietlPustyWpis(ui->textLista_nas);

    ui->btnPoprzedni->setEnabled(widok.maPoprzedni);
    ui->btnNastepny->setEnabled(widok.maNastepny);
    ui->btnUsun->setEnabled(widok.moznaUsunac);
}

void MainWindow::wyswietlWpisWPolu(QTextEdit* pole, const Wpis& wpis) {
    pole->clear();

    QString dataString = wpis.getDataWpisu().toString("dd.MM.yyyy hh:mm");
    QString trescString = wpis.getTresc();
    QString emocjaString = Program::emocjaDoTekstu(wpis.getEmocja());
    pole->append("<b>" + dataString + "</b> <i>" + emocjaString + "</i>");
    pole->append(trescString);
}

void MainWindow::wyswietlPustyWpis(QTextEdit* pole) {
    pole->clear();
    pole->append("<div style='color: gray; font-style: italic; text-align: center; margin-top: 10px;'>"
                 "Brak wspomnienia"
                 "</div>");
}

void MainWindow::zastosujStyle() {
    QString styl = QString("QTextEdit { background-color: %1; color: %2; }")
                       .arg(m_kolorTla.name())
                       .arg(m_kolorCzcionki.name());

    ui->textLista_pop->setStyleSheet(styl);
    ui->textLista_akt->setStyleSheet(styl);
    ui->textLista_nas->setStyleSheet(styl);

    ui->textLista_pop->setFont(m_czcionka);
    ui->textLista_akt->setFont(m_czcionka);
    ui->textLista_nas->setFont(m_czcionka);
}

void MainWindow::ustawDomyslneStyleWizualne() {
    m_kolorTla = Qt::white;
    m_kolorCzcionki = Qt::black;
    m_czcionka = QFont("Arial", 10);
}

void MainWindow::slotZmienionoWyglad(const QFont& font, const QColor& tlo, const QColor& czcionka) {
    m_czcionka = font;
    m_kolorTla = tlo;
    m_kolorCzcionki = czcionka;
    zastosujStyle();
    aktualizujWyswietlanie();
}

void MainWindow::wykonajTesty(QDebug &diag)
{
    using WskNaTest = bool (MainWindow::*)(QDebug&);

    WskNaTest testy[] = {
        &MainWindow::testFiltracjiEmocji
    };

    for (auto test : testy) {
        if ((this->*test)(diag)) {
            diag << " [PASSED]\n";
        } else {
            diag << " [FAILED!]\n";
        }
    }
}

void MainWindow::przygotujDaneTestowe9Wpisow()
{
    Repozytorium* repo = m_program->m_repo;
    qDeleteAll(repo->m_wszystkie);
    repo->m_wszystkie.clear();
    repo->m_widoczne.clear();

    QDateTime dt = QDateTime::currentDateTime();

    repo->m_wszystkie.append(new Wpis(dt, "Smutny 1", Emocja::Smutne));
    repo->m_wszystkie.append(new Wpis(dt, "Smutny 2", Emocja::Smutne));
    repo->m_wszystkie.append(new Wpis(dt, "Smutny 3", Emocja::Smutne));

    repo->m_wszystkie.append(new Wpis(dt, "Neutralny 1", Emocja::Neutralne));
    repo->m_wszystkie.append(new Wpis(dt, "Neutralny 2", Emocja::Neutralne));
    repo->m_wszystkie.append(new Wpis(dt, "Neutralny 3", Emocja::Neutralne));

    repo->m_wszystkie.append(new Wpis(dt, "Radosny 1", Emocja::Radosne));
    repo->m_wszystkie.append(new Wpis(dt, "Radosny 2", Emocja::Radosne));
    repo->m_wszystkie.append(new Wpis(dt, "Radosny 3", Emocja::Radosne));
}

bool MainWindow::testFiltracjiEmocji(QDebug &diag)
{
    diag << "TEST: Filtracja (Smutne=TAK, Radosne=TAK, Neutralne=NIE)... ";

    przygotujDaneTestowe9Wpisow();

    ui->checkSmutne->setChecked(true);
    ui->checkNeutralne->setChecked(false);
    ui->checkRadosne->setChecked(true);

    int liczbaWidocznych = m_program->pobierzLiczbeWpisow();

    if (liczbaWidocznych != 6) {
        diag << "\n   BLAD: Oczekiwano 6 wpisow, otrzymano: " << liczbaWidocznych;
        return false;
    }

    for (int i = 0; i < liczbaWidocznych; ++i) {
        const Wpis& w = m_program->pobierzWpis(i);
        if (w.getEmocja() == Emocja::Neutralne) {
            diag << "\n   BLAD: Znaleziono wpis Neutralny, a mial byc odfiltrowany!";
            return false;
        }
    }

    return true;
}
