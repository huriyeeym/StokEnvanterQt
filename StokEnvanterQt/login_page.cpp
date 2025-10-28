#include "login_page.h"
#include <QMessageBox>
#include "mainwindow.h"

LoginPage::LoginPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::login_page)  // Ui::login_page olarak değiştirdik
{
    ui->setupUi(this);
    ui->passwordLineEdit->setEchoMode(QLineEdit::Password);
    loadUserTypes();
    connect(ui->loginButton, &QPushButton::clicked, this, &LoginPage::onLoginButtonClicked);
}

LoginPage::~LoginPage()
{
    delete ui;
}

void LoginPage::onLoginButtonClicked()
{
    QString username = ui->usernameLineEdit->text();
    QString password = ui->passwordLineEdit->text();
    QString userType = ui->userTypeComboBox->currentText();
    qDebug() << "Login attempt with userType:" << userType;

    if (!dbManager.connectToDatabase()) {
        QMessageBox::critical(this, tr("Hata"), tr("Veritabanına bağlanılamadı!"));
        return;
    }

    QSqlQuery query(dbManager.getDatabase());
    query.prepare("SELECT * FROM Users WHERE Username = ? AND Password = ? AND UserType = ?");
    query.addBindValue(username);
    query.addBindValue(password);
    query.addBindValue(userType);

    if (query.exec() && query.next()) {
        qDebug() << "Login successful, emitting signal with userType:" << userType;

        QWidget* parentWidget = qobject_cast<QWidget*>(parent());
        qDebug() << "Parent widget is:" << (parentWidget ? parentWidget->metaObject()->className() : "nullptr");

        MainWindow* mainWin = qobject_cast<MainWindow*>(parentWidget);
        if (mainWin) {
            qDebug() << "Successfully accessed MainWindow";
            mainWin->setUserTypeAndInitialize(userType);
        } else {
            qDebug() << "Warning: Could not access MainWindow. Parent type:"
                     << (parent() ? parent()->metaObject()->className() : "nullptr");
        }

        emit loginSuccessful(userType);
    } else {
        QMessageBox::warning(this, tr("Hata"), tr("Geçersiz giriş bilgileri!"));
    }
}

void LoginPage::loadUserTypes()
{
    if (!dbManager.connectToDatabase()) {
        QMessageBox::critical(this, tr("Hata"), tr("Veritabanına bağlanılamadı!"));
        return;
    }

    QSqlQuery query(dbManager.getDatabase());
    query.exec("SELECT DISTINCT UserType FROM Users");

    ui->userTypeComboBox->clear();
    while (query.next()) {
        ui->userTypeComboBox->addItem(query.value("UserType").toString());
    }
}
