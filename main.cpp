#include "W_PREZENTACJI/mainwindow.h"
#include "W_USLUG/Program.h"
#include <QApplication>
#include <QDebug>

#define DIAG 0

#if(DIAG == 0)
// TRYB NORMALNY
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Program program;
    MainWindow w(nullptr, &program);
    program.start(); // Musi być po otwarciu okna
    w.show();
    return a.exec();
}

#else
int main(int argc, char *argv[])
{
    QDebug diag(QtDebugMsg);
    diag.nospace();
    diag.noquote();

    QApplication a(argc, argv); // Konieczne dla GUI
    Program program;
    MainWindow w(nullptr, &program);
    diag << "Rozpoczecie testow:";
    w.wykonajTesty(diag);
    diag << "Zakonczenie testow\n";
    return 0;
}
#endif
