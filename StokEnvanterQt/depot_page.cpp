#include "depot_page.h"
#include "ui_depot_page.h"
#include <QMessageBox>
#include <QSqlError>
#include <QSqlRecord>

DepotPage::DepotPage(QWidget *parent, const QString &userType)
    : QWidget(parent)
    , ui(new Ui::DepotPage)
    , model(new QSqlTableModel(this))
    , isNewDepot(true)
    , userType(userType)
{
    ui->setupUi(this);

    // Başlangıçta ana görünümü (tablo) göster
    ui->stackedWidget->setCurrentIndex(1);  // page_2 (tablo görünümü)

    setupModel();
    setupConnections();
    setupComboBoxes();
    loadDepotData();
}


void DepotPage::setupModel()
{
    if (!dbManager.connectToDatabase()) {
        QMessageBox::critical(this, tr("Hata"), tr("Veritabanına bağlanılamadı!"));
        return;
    }

    model->setTable("Depots");
    model->setHeaderData(0, Qt::Horizontal, tr("Depo ID"));
    model->setHeaderData(1, Qt::Horizontal, tr("Depo Adı"));
    model->setHeaderData(2, Qt::Horizontal, tr("Şube"));
    model->setHeaderData(3, Qt::Horizontal, tr("Yönetici"));
    model->setHeaderData(4, Qt::Horizontal, tr("Kapasite"));

    ui->depotTableView->setModel(model);
    ui->depotTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->depotTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->depotTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void DepotPage::setupConnections()
{
    // Ana butonlar
    connect(ui->addButton, &QPushButton::clicked, this, &DepotPage::showAddDepotForm);
    connect(ui->deleteButton, &QPushButton::clicked, this, &DepotPage::deleteDepot);
    connect(ui->updateButton, &QPushButton::clicked, this, &DepotPage::showUpdateDepotForm);

    // Form butonları
    connect(ui->saveButton, &QPushButton::clicked, this, &DepotPage::saveDepot);
    connect(ui->cancelButton, &QPushButton::clicked, this, &DepotPage::cancelForm);
}

void DepotPage::switchToMainView()
{
    // Ana görünüme geç (tablo ve butonlar)
    ui->stackedWidget->setCurrentIndex(1);  // page_2
    clearForm();
}

void DepotPage::switchToFormView()
{
    // Form görünümüne geç
    ui->stackedWidget->setCurrentIndex(0);  // page
}


void DepotPage::setupComboBoxes()
{
    // Image 1'de görülen şube listesine göre dolduruyoruz
    ui->branchComboBox->addItems({"Merkez", "Ankara", "İstanbul", "İzmir", "Bursa", "Antalya"});
}


void DepotPage::showAddDepotForm()
{
    isNewDepot = true;
    clearForm();
    switchToFormView();
}
void DepotPage::showUpdateDepotForm()
{
    QModelIndex currentIndex = ui->depotTableView->currentIndex();
    if (!currentIndex.isValid()) {
        QMessageBox::warning(this, tr("Uyarı"), tr("Lütfen güncellenecek depoyu seçin."));
        return;
    }

    isNewDepot = false;

    // Seçili deponun bilgilerini forma doldur
    ui->depotNameEdit->setText(model->data(model->index(currentIndex.row(), 1)).toString());
    ui->branchComboBox->setCurrentText(model->data(model->index(currentIndex.row(), 2)).toString());
    ui->managerEdit->setText(model->data(model->index(currentIndex.row(), 3)).toString());
    ui->capacitySpinBox->setValue(model->data(model->index(currentIndex.row(), 4)).toInt());

    switchToFormView();
}

void DepotPage::saveDepot()
{
    // Form validasyonu
    if (ui->depotNameEdit->text().isEmpty()) {
        QMessageBox::warning(this, tr("Uyarı"), tr("Depo adı boş olamaz!"));
        return;
    }

    if (isNewDepot) {
        QSqlRecord record = model->record();
        record.remove(record.indexOf("DepotID")); // Auto-increment olduğu için ID'yi kaldır

        record.setValue("DepotName", ui->depotNameEdit->text());
        record.setValue("Branch", ui->branchComboBox->currentText());
        record.setValue("Manager", ui->managerEdit->text());
        record.setValue("Capacity", ui->capacitySpinBox->value());

        if (!model->insertRecord(-1, record)) {
            QMessageBox::critical(this, tr("Hata"), tr("Depo eklenemedi: ") + model->lastError().text());
            return;
        }
    } else {
        QModelIndex currentIndex = ui->depotTableView->currentIndex();
        model->setData(model->index(currentIndex.row(), 1), ui->depotNameEdit->text());
        model->setData(model->index(currentIndex.row(), 2), ui->branchComboBox->currentText());
        model->setData(model->index(currentIndex.row(), 3), ui->managerEdit->text());
        model->setData(model->index(currentIndex.row(), 4), ui->capacitySpinBox->value());
    }

    if (!model->submitAll()) {
        QMessageBox::critical(this, tr("Hata"), tr("Değişiklikler kaydedilemedi: ") + model->lastError().text());
        model->revertAll();
        return;
    }

    loadDepotData();
    switchToMainView();
}



void DepotPage::clearForm()
{
    ui->depotNameEdit->clear();
    ui->branchComboBox->setCurrentIndex(0);
    ui->managerEdit->clear();
    ui->capacitySpinBox->setValue(0);
}

void DepotPage::cancelForm()
{
    switchToMainView();
}

void DepotPage::loadDepotData()
{
    if (!model->select()) {
        QMessageBox::critical(this, tr("Hata"), tr("Veriler yüklenemedi: ") + model->lastError().text());
        return;
    }
    ui->depotTableView->resizeColumnsToContents();
}
void DepotPage::deleteDepot()
{
    qDebug() << "deleteDepot called - userType:" << userType;

    // Yetki kontrolü
    if (userType == "Çalışan") {
        qDebug() << "Delete operation blocked - Employee user";
        QMessageBox::warning(this, tr("Yetki Hatası"),
                             tr("Çalışan kullanıcılar depo silme yetkisine sahip değildir!"));
        return;
    }

    QModelIndex currentIndex = ui->depotTableView->currentIndex();
    if (!currentIndex.isValid()) {
        QMessageBox::warning(this, tr("Uyarı"), tr("Lütfen silinecek depoyu seçin."));
        return;
    }

    QString depotName = model->data(model->index(currentIndex.row(), 1)).toString();

    QMessageBox::StandardButton reply = QMessageBox::question(this, tr("Depo Sil"),
                                                              tr("\"%1\" deposunu silmek istediğinize emin misiniz?").arg(depotName),
                                                              QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No)
        return;

    if (!model->removeRow(currentIndex.row())) {
        QMessageBox::critical(this, tr("Hata"), tr("Depo silinemedi: ") + model->lastError().text());
        return;
    }

    if (!model->submitAll()) {
        QMessageBox::critical(this, tr("Hata"), tr("Değişiklikler kaydedilemedi: ") + model->lastError().text());
        model->revertAll();
        return;
    }

    loadDepotData();
}
DepotPage::~DepotPage()
{
    delete ui;
}
