#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class Program;
class Wpis;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr, Program* program = nullptr);
    ~MainWindow();

    void wykonajTesty(QDebug &diag);

private slots:
    // Reakcje na przyciski gui
    void on_btnPoprzedni_clicked();
    void on_btnNastepny_clicked();
    void on_btnUsun_clicked();
    void on_btnDodajWpis_clicked();
    void on_btnSzukaj_clicked();
    void on_btnOpcje_clicked();

    // Checkboxy filtracji
    void on_checkSmutne_toggled(bool checked);
    void on_checkNeutralne_toggled(bool checked);
    void on_checkRadosne_toggled(bool checked);

    // Reakcje na komunikaty z warstwy usług
    void slotOdswiezWidok();
    void slotPokazStatus(QString komunikat);
    void slotPrzejdzDoIndeksu(int indeks);

    void slotZmienionoWyglad(const QFont& font, const QColor& tlo, const QColor& czcionka);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    Ui::MainWindow *ui;
    Program *m_program;

    // Ustawienia wizualne
    QColor m_kolorTla;
    QColor m_kolorCzcionki;
    QFont m_czcionka;

    // Metody prezentacyjne
    void wyswietlWpisWPolu(QTextEdit* pole, const Wpis& wpis);

    void wyswietlPustyWpis(QTextEdit* pole);
    void aktualizujWyswietlanie();
    void zastosujStyle();
    void ustawDomyslneStyleWizualne();

    bool testFiltracjiEmocji(QDebug &diag);
    void przygotujDaneTestowe9Wpisow();
};

#endif // MAINWINDOW_H
