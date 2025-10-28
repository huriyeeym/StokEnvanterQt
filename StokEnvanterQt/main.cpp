#include "mainwindow.h"
#include "login_page.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    LoginPage loginPage;
    loginPage.setWindowTitle("Stok Envanter Sistemi - Giriş");
    loginPage.show();

    MainWindow w;
    QObject::connect(&loginPage, &LoginPage::loginSuccessful,
                     [&w, &loginPage](const QString &userType) {
                         qDebug() << "Login successful, setting userType:" << userType;
                         w.setUserTypeAndInitialize(userType);  // Önce userType'ı ayarla
                         loginPage.close();
                         w.show();
                         if(userType == "Çalışan") {
                             w.findChild<QPushButton*>("settingsButton")->hide();
                         }
                     });

    return a.exec();
}
