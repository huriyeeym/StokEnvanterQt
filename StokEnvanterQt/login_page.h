#ifndef LOGIN_PAGE_H
#define LOGIN_PAGE_H

#include <QWidget>
#include <QSqlQuery>
#include "database_manager.h"
#include "ui_login_page.h"  // Burada ui dosyasını include ediyoruz
class MainWindow;
namespace Ui {
class login_page;  // UI sınıfı için forward declaration
}

class LoginPage : public QWidget {
    Q_OBJECT

public:
    explicit LoginPage(QWidget *parent = nullptr);
    ~LoginPage();

signals:
    void loginSuccessful(const QString &userType);

private slots:
    void onLoginButtonClicked();
    void loadUserTypes();

private:
    Ui::login_page *ui;  // Ui namespace'indeki login_page sınıfını kullanıyoruz
    DatabaseManager dbManager;
};

#endif // LOGIN_PAGE_H
