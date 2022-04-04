#include <QApplication>
#include "main_window.h"

#include <clocale>
#include <Windows.h>

int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "PL_pl.UTF-8");
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    QApplication a(argc, argv);
    gno_main_window w;
    w.show();
    w.adjustSize();
    return a.exec();
}
