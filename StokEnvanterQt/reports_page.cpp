#include "reports_page.h"
#include "ui_reports_page.h"
#include <QMessageBox>
#include <qpdfwriter.h>

ReportsPage::ReportsPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ReportsPage)
    , stockModel(nullptr)
    , depotModel(nullptr)
    , financialModel(nullptr)
{
    ui->setupUi(this);
    setupUI();
    setupConnections();
    setupModels();
}

ReportsPage::~ReportsPage()
{
    delete ui;
    delete stockModel;
    delete depotModel;
    delete financialModel;
}

void ReportsPage::setupUI()
{
    // Stok tablosu için görünüm ayarları
    ui->stockTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->stockTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->stockTableView->setAlternatingRowColors(true);
    ui->stockTableView->verticalHeader()->setVisible(false);
    ui->stockTableView->setShowGrid(true);

    // ComboBox'a kategorileri ekle - Stok için
    ui->stockCategoryCombo->addItem("Tümü");
    QSqlQuery stockQuery(dbManager.getDatabase());
    stockQuery.exec("SELECT DISTINCT Category FROM Products ORDER BY Category");
    while (stockQuery.next()) {
        ui->stockCategoryCombo->addItem(stockQuery.value(0).toString());
    }

    // Depo ComboBox'a şubeleri ekle
    ui->depotBranchCombo->addItem("Tümü");
    QSqlQuery depotQuery(dbManager.getDatabase());
    depotQuery.exec("SELECT DISTINCT Branch FROM Depots ORDER BY Branch");
    while (depotQuery.next()) {
        ui->depotBranchCombo->addItem(depotQuery.value(0).toString());
    }

    // Tarih alanları için
    ui->stockStartDate->setDate(QDate::currentDate().addMonths(-1));
    ui->stockEndDate->setDate(QDate::currentDate());
    ui->depotStartDate->setDate(QDate::currentDate().addMonths(-1));
    ui->depotEndDate->setDate(QDate::currentDate());

    // GraphicsView ayarları - Stok için
    if (!ui->stockGraphicsView->scene()) {
        ui->stockGraphicsView->setScene(new QGraphicsScene(this));
    }
    ui->stockGraphicsView->setRenderHint(QPainter::Antialiasing);
    ui->stockGraphicsView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    ui->stockGraphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->stockGraphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // GraphicsView ayarları - Depo için
    if (!ui->depotGraphicsView->scene()) {
        ui->depotGraphicsView->setScene(new QGraphicsScene(this));
    }
    ui->depotGraphicsView->setRenderHint(QPainter::Antialiasing);
    ui->depotGraphicsView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    ui->depotGraphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->depotGraphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // setupUI() fonksiyonuna eklenecek kısım
    ui->financeTypeCombo->clear();
    ui->financeTypeCombo->addItems({"Gelir Raporu", "Gider Raporu", "Kar/Zarar Raporu"});
    ui->financeStartDate->setDate(QDate::currentDate().addMonths(-1));
    ui->financeEndDate->setDate(QDate::currentDate());
}

void ReportsPage::setupConnections()
{
    // Stok raporu bağlantıları
    connect(ui->stockGenerateButton, &QPushButton::clicked,
            this, &ReportsPage::generateStockReport);
    connect(ui->stockExcelButton, &QPushButton::clicked,
            this, &ReportsPage::exportStockToExcel);
    connect(ui->stockPdfButton, &QPushButton::clicked,
            this, &ReportsPage::exportStockToPDF);

    // Depo raporu bağlantıları
    connect(ui->depotGenerateButton, &QPushButton::clicked,
            this, &ReportsPage::generateDepotReport);
    connect(ui->depotExcelButton, &QPushButton::clicked,
            this, &ReportsPage::exportDepotToExcel);
    connect(ui->depotPdfButton, &QPushButton::clicked,
            this, &ReportsPage::exportDepotToPDF);

    // Finansal rapor bağlantıları
    connect(ui->financeGenerateButton, &QPushButton::clicked,
            this, &ReportsPage::generateFinancialReport);
    connect(ui->financeExcelButton, &QPushButton::clicked,
            this, &ReportsPage::exportFinancialToExcel);
    connect(ui->financePdfButton, &QPushButton::clicked,
            this, &ReportsPage::exportFinancialToPDF);
    connect(ui->financeTypeCombo, &QComboBox::currentTextChanged,
            this, &ReportsPage::onFinanceTypeChanged);
    connect(ui->depotBranchCombo, &QComboBox::currentTextChanged,
            this, &ReportsPage::updateDepotTable);

}

void ReportsPage::setupModels()
{
    // Stok model ayarları
    stockModel = new QSqlTableModel(this);
    stockModel->setTable("Products");

    // Sütun başlıklarını ayarla
    stockModel->setHeaderData(0, Qt::Horizontal, tr("Ürün ID"));
    stockModel->setHeaderData(1, Qt::Horizontal, tr("Ürün Kodu"));
    stockModel->setHeaderData(2, Qt::Horizontal, tr("Ürün Adı"));
    stockModel->setHeaderData(3, Qt::Horizontal, tr("Kategori"));
    stockModel->setHeaderData(4, Qt::Horizontal, tr("Fiyat"));
    stockModel->setHeaderData(5, Qt::Horizontal, tr("Stok"));
    stockModel->setHeaderData(6, Qt::Horizontal, tr("Durum"));
    stockModel->setHeaderData(7, Qt::Horizontal, tr("Depo"));

    ui->stockTableView->setModel(stockModel);

    depotModel = new QSqlTableModel(this);
    depotModel->setTable("Depots");

    // Sütun başlıklarını ayarla
    depotModel->setHeaderData(0, Qt::Horizontal, tr("Depo ID"));
    depotModel->setHeaderData(1, Qt::Horizontal, tr("Depo Adı"));
    depotModel->setHeaderData(2, Qt::Horizontal, tr("Şube"));
    depotModel->setHeaderData(3, Qt::Horizontal, tr("Yönetici"));
    depotModel->setHeaderData(4, Qt::Horizontal, tr("Kapasite"));

    ui->depotTableView->setModel(depotModel);

    financialModel = new QSqlTableModel(this);
    financialModel->setTable("Products");  // Geçici olarak Products tablosunu kullanıyoruz

    // Sütun başlıklarını ayarla
    financialModel->setHeaderData(0, Qt::Horizontal, tr("İşlem ID"));
    financialModel->setHeaderData(1, Qt::Horizontal, tr("Tarih"));
    financialModel->setHeaderData(2, Qt::Horizontal, tr("İşlem Tipi"));
    financialModel->setHeaderData(3, Qt::Horizontal, tr("Ürün"));
    financialModel->setHeaderData(4, Qt::Horizontal, tr("Miktar"));
    financialModel->setHeaderData(5, Qt::Horizontal, tr("Birim Fiyat"));
    financialModel->setHeaderData(6, Qt::Horizontal, tr("Toplam"));
    financialModel->setHeaderData(7, Qt::Horizontal, tr("Açıklama"));

    ui->financeTableView->setModel(financialModel);
}

// Stok Raporu Fonksiyonları
void ReportsPage::generateStockReport()
{
    // Önce grafik scene'i temizle
    if (!ui->stockGraphicsView->scene()) {
        ui->stockGraphicsView->setScene(new QGraphicsScene(this));
    }
    ui->stockGraphicsView->scene()->clear();

    // Verileri çek
    QSqlQuery query(dbManager.getDatabase());
    QString queryStr = "SELECT Products.ProductName, Products.Stock "
                       "FROM Products "
                       "WHERE Products.Category = :category "
                       "AND Products.Stock > 0";

    QString selectedCategory = ui->stockCategoryCombo->currentText();
    if (selectedCategory == "Tümü") {
        queryStr = "SELECT Products.ProductName, Products.Stock "
                   "FROM Products "
                   "WHERE Products.Stock > 0";
    }

    if (!query.prepare(queryStr)) {
        QMessageBox::critical(this, tr("Hata"),
                              tr("Sorgu hazırlanırken hata oluştu: %1").arg(query.lastError().text()));
        return;
    }

    if (selectedCategory != "Tümü") {
        query.bindValue(":category", selectedCategory);
    }

    if (!query.exec()) {
        QMessageBox::critical(this, tr("Hata"),
                              tr("Sorgu çalıştırılırken hata oluştu: %1").arg(query.lastError().text()));
        return;
    }

    // Grafik çizimi için ayarlar
    QGraphicsScene* scene = ui->stockGraphicsView->scene();
    const int maxHeight = 200;  // Grafik yüksekliği
    const int barWidth = 40;    // Çubuk genişliği
    const int spacing = 20;     // Çubuklar arası boşluk
    int xPos = 50;             // Başlangıç x pozisyonu (sol boşluk için)
    int yPos = maxHeight + 20;  // Y ekseni uzunluğu (altdaki metinler için boşluk azaltıldı)
    const int textSpace = 60;   // Metin için ekstra boşluk

    // Önce maksimum stok değerini bul
    int maxStock = 0;
    while (query.next()) {
        int stock = query.value(1).toInt();
        if (stock > maxStock) maxStock = stock;
    }

    // Y ekseni ve değerleri
    scene->addLine(xPos, 50, xPos, yPos, QPen(Qt::white));
    for (int i = 0; i <= 5; i++) {
        int y = yPos - (i * maxHeight / 5);
        // Grid çizgisi
        scene->addLine(xPos - 5, y, xPos + 500, y, QPen(QColor(255, 255, 255, 50)));
        // Y ekseni değerleri
        QGraphicsTextItem* valueText = scene->addText(QString::number(maxStock * i / 5));
        valueText->setDefaultTextColor(Qt::white);
        valueText->setPos(5, y - 10);
    }

    // X ekseni çiz
    scene->addLine(xPos, yPos, xPos + 450, yPos, QPen(Qt::white));

    // Sorguyu başa sar
    query.first();
    query.previous();

    // Grafik çizimi
    while (query.next()) {
        QString productName = query.value(0).toString();
        int stock = query.value(1).toInt();

        // Çubuk yüksekliğini hesapla
        int barHeight = maxStock > 0 ? (stock * maxHeight) / maxStock : 0;

        // Çubuk çiz
        QRectF barRect(xPos, yPos - barHeight, barWidth, barHeight);
        scene->addRect(barRect, QPen(Qt::white), QBrush(QColor(0, 153, 255)));

        // Ürün adını yaz - tablonun altında
        QGraphicsTextItem* nameText = scene->addText(productName);
        nameText->setDefaultTextColor(Qt::white);
        nameText->setRotation(-90);
        nameText->setPos(xPos + (barWidth/2) - (nameText->boundingRect().height()/2),
                         yPos + textSpace); // textSpace kadar aşağıya kaydır

        // Stok değerini çubuğun üzerine yaz
        QGraphicsTextItem* stockText = scene->addText(QString::number(stock));
        stockText->setDefaultTextColor(Qt::white);
        stockText->setPos(xPos + (barWidth - stockText->boundingRect().width()) / 2,
                          yPos - barHeight - 20);

        xPos += barWidth + spacing;
    }

    // Scene boyutunu ayarla (alt metinler için ekstra alan)
    QRectF rect = scene->itemsBoundingRect();
    rect.setHeight(rect.height() + textSpace); // Alt metinler için ekstra alan
    scene->setSceneRect(rect);

    // View'ı güncelle
    ui->stockGraphicsView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);

    // Tabloyu güncelle
    updateStockTable();
}

void ReportsPage::updateStockTable()
{
    if (!stockModel) {
        stockModel = new QSqlTableModel(this, dbManager.getDatabase());
    }

    stockModel->setTable("Products");

    // Filtreler için gerekli değerleri al
    QDate startDate = ui->stockStartDate->date();
    QDate endDate = ui->stockEndDate->date();
    QString selectedCategory = ui->stockCategoryCombo->currentText();

    // Filtre oluştur
    QString filter;

    // Tarih filtresi ve kategori filtresi
    if (selectedCategory != "Tümü") {
        filter = QString("Category = '%1' AND CONVERT(date, Tarih) BETWEEN '%2' AND '%3'")
        .arg(selectedCategory)
            .arg(startDate.toString("yyyy-MM-dd"))
            .arg(endDate.toString("yyyy-MM-dd"));
    } else {
        filter = QString("CONVERT(date, Tarih) BETWEEN '%1' AND '%2'")
        .arg(startDate.toString("yyyy-MM-dd"))
            .arg(endDate.toString("yyyy-MM-dd"));
    }

    stockModel->setFilter(filter);

    // Sütun başlıklarını ayarla
    stockModel->setHeaderData(0, Qt::Horizontal, tr("Ürün ID"));
    stockModel->setHeaderData(1, Qt::Horizontal, tr("Ürün Kodu"));
    stockModel->setHeaderData(2, Qt::Horizontal, tr("Ürün Adı"));
    stockModel->setHeaderData(3, Qt::Horizontal, tr("Kategori"));
    stockModel->setHeaderData(4, Qt::Horizontal, tr("Fiyat"));
    stockModel->setHeaderData(5, Qt::Horizontal, tr("Stok"));
    stockModel->setHeaderData(6, Qt::Horizontal, tr("Durum"));
    stockModel->setHeaderData(7, Qt::Horizontal, tr("Depo ID"));
    stockModel->setHeaderData(8, Qt::Horizontal, tr("Tarih"));

    if (!stockModel->select()) {
        qDebug() << "SQL Error:" << stockModel->lastError().text();
        QMessageBox::critical(this, tr("Hata"),
                              tr("Veriler yüklenirken hata oluştu: %1").arg(stockModel->lastError().text()));
        return;
    }

    ui->stockTableView->setModel(stockModel);

    // Tablo görünüm ayarları
    ui->stockTableView->setStyleSheet(
        "QTableView {"
        "    background-color: #1e1e2f;"
        "    alternate-background-color: #252540;"
        "    color: #ecf0f1;"
        "    border: none;"
        "    border-radius: 10px;"
        "    gridline-color: transparent;"
        "}"
        "QHeaderView::section {"
        "    background-color: #1a237e;"
        "    padding: 8px;"
        "    border: none;"
        "    color: white;"
        "    font-weight: bold;"
        "    border-bottom: 2px solid #3F51B5;"
        "}"
        "QTableView::item {"
        "    border: none;"
        "    border-bottom: 1px solid #34344a;"
        "    padding: 5px;"
        "}"
        );

    // Diğer tablo ayarları
    ui->stockTableView->setShowGrid(false);
    ui->stockTableView->setAlternatingRowColors(true);
    ui->stockTableView->verticalHeader()->setVisible(false);
    ui->stockTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->stockTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->stockTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Sütun genişlikleri
    ui->stockTableView->setColumnWidth(0, 70);  // Ürün ID
    ui->stockTableView->setColumnWidth(1, 100); // Ürün Kodu
    ui->stockTableView->setColumnWidth(2, 150); // Ürün Adı
    ui->stockTableView->setColumnWidth(3, 100); // Kategori
    ui->stockTableView->setColumnWidth(4, 80);  // Fiyat
    ui->stockTableView->setColumnWidth(5, 70);  // Stok
    ui->stockTableView->setColumnWidth(6, 80);  // Durum
    ui->stockTableView->setColumnWidth(7, 70);  // Depo ID
    ui->stockTableView->setColumnWidth(8, ui->stockTableView->width() - 720); // Tarih
}


void ReportsPage::exportStockToExcel()
{
    QString fileName = getSaveFilePath("Excel Files (*.csv)");
    if (!fileName.isEmpty()) {
        if (!fileName.endsWith(".csv")) {
            fileName += ".csv";
        }
        exportToExcel(ui->stockTableView, fileName);
    }
}

void ReportsPage::exportStockToPDF()
{
    QString fileName = getSaveFilePath("PDF Files (*.pdf)");
    if (!fileName.isEmpty()) {
        if (!fileName.endsWith(".pdf")) {
            fileName += ".pdf";
        }
        exportToPDF(ui->stockTableView, fileName);
    }
}

void ReportsPage::generateDepotReport()
{
    // Başlangıç ve bitiş tarihlerini al
    QDate startDate = ui->depotStartDate->date();
    QDate endDate = ui->depotEndDate->date();

    if (!ui->depotGraphicsView->scene()) {
        ui->depotGraphicsView->setScene(new QGraphicsScene(this));
    }
    ui->depotGraphicsView->scene()->clear();

    QSqlQuery query(dbManager.getDatabase());
    QString queryStr = "SELECT d.DepotName, d.Capacity, "
                       "(SELECT SUM(p.Stock) FROM Products p "
                       "WHERE p.DepotID = d.DepotID) as TotalStock "
                       "FROM Depots d";

    QString selectedBranch = ui->depotBranchCombo->currentText();
    if (selectedBranch != "Tümü") {
        queryStr += " WHERE d.Branch = :branch";
    }

    if (!query.prepare(queryStr)) {
        QMessageBox::critical(this, tr("Hata"), tr("Sorgu hazırlanırken hata oluştu!"));
        return;
    }

    query.bindValue(":startDate", startDate.toString("yyyy-MM-dd"));
    query.bindValue(":endDate", endDate.toString("yyyy-MM-dd"));
    if (selectedBranch != "Tümü") {
        query.bindValue(":branch", selectedBranch);
    }

    if (!query.exec()) {
        QMessageBox::critical(this, tr("Hata"), tr("Sorgu çalıştırılırken hata oluştu!"));
        return;
    }

    QGraphicsScene* scene = ui->depotGraphicsView->scene();
    const int maxHeight = 200;
    const int barWidth = 40;
    const int spacing = 20;
    int xPos = 50;
    int yPos = maxHeight + 20;
    const int textSpace = 80;  // Artırıldı - metin alanı için daha fazla boşluk

    // Y ekseni ve grid çizgileri
    scene->addLine(xPos, 50, xPos, yPos, QPen(Qt::white));
    for (int i = 0; i <= 5; i++) {
        int y = yPos - (i * maxHeight / 5);
        // Grid çizgisi
        scene->addLine(xPos - 5, y, xPos + 600, y, QPen(QColor(255, 255, 255, 30)));
        // Yüzde değeri
        QString percentText = QString("%1%").arg(i * 20);
        QGraphicsTextItem* valueText = scene->addText(percentText);
        valueText->setDefaultTextColor(Qt::white);
        valueText->setPos(5, y - 10);
    }

    // X ekseni çiz
    scene->addLine(xPos, yPos, xPos + 600, yPos, QPen(Qt::white));

    while (query.next()) {
        QString depotName = query.value("DepotName").toString();
        int capacity = query.value("Capacity").toInt();
        int totalStock = query.value("TotalStock").isNull() ? 0 : query.value("TotalStock").toInt();

        double fillRate = capacity > 0 ? (totalStock * 100.0) / capacity : 0;
        int barHeight = (fillRate * maxHeight) / 100;

        // Renk seçimi
        QColor barColor;
        if (fillRate < 50)
            barColor = QColor(52, 152, 219);      // Mavi
        else if (fillRate < 80)
            barColor = QColor(241, 196, 15);      // Sarı
        else
            barColor = QColor(231, 76, 60);       // Kırmızı

        // Bar çiz
        QRectF barRect(xPos, yPos - barHeight, barWidth, barHeight);
        scene->addRect(barRect, QPen(Qt::white), QBrush(barColor));

        // Doluluk oranı
        QString rateText = QString("%1%").arg(qRound(fillRate));
        QGraphicsTextItem* rateTextItem = scene->addText(rateText);
        rateTextItem->setDefaultTextColor(Qt::white);
        rateTextItem->setPos(xPos + (barWidth - rateTextItem->boundingRect().width()) / 2,
                             yPos - barHeight - 20);

        // Kapasite bilgisi
        QString stockText = QString("%1/%2").arg(totalStock).arg(capacity);
        QGraphicsTextItem* stockTextItem = scene->addText(stockText);
        stockTextItem->setDefaultTextColor(Qt::white);
        stockTextItem->setPos(xPos + (barWidth - stockTextItem->boundingRect().width()) / 2,
                              yPos - barHeight - 40);

        const int labelDistance = 90;
        // Depo adı - daha aşağıya yerleştir
        QGraphicsTextItem* nameText = scene->addText(depotName);
        nameText->setDefaultTextColor(Qt::white);
        nameText->setRotation(-90);
        nameText->setPos(xPos + (barWidth/2) - (nameText->boundingRect().height()/2),
                         yPos + labelDistance);  // labelDistance kadar aşağı

        xPos += barWidth + spacing;
    }

    // Scene boyutunu ayarla (alt metinler için ekstra alan)
    QRectF rect = scene->itemsBoundingRect();
    rect.setHeight(rect.height() + textSpace);  // Alt metinler için alan artırıldı
    scene->setSceneRect(rect);
    ui->depotGraphicsView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);

    updateDepotTable();
}



void ReportsPage::updateDepotTable()
{
    if (!depotModel) {
        depotModel = new QSqlTableModel(this, dbManager.getDatabase());
    }

    depotModel->setTable("Depots");
    QString selectedBranch = ui->depotBranchCombo->currentText();

    // Sadece şube filtresi uygula
    if (selectedBranch != "Tümü") {
        depotModel->setFilter(QString("Branch = '%1'").arg(selectedBranch));
    } else {
        depotModel->setFilter("");
    }

    // Başlıkları ayarla
    depotModel->setHeaderData(0, Qt::Horizontal, tr("Depo ID"));
    depotModel->setHeaderData(1, Qt::Horizontal, tr("Depo Adı"));
    depotModel->setHeaderData(2, Qt::Horizontal, tr("Şube"));
    depotModel->setHeaderData(3, Qt::Horizontal, tr("Yönetici"));
    depotModel->setHeaderData(4, Qt::Horizontal, tr("Kapasite"));

    if (!depotModel->select()) {
        QMessageBox::critical(this, tr("Hata"),
                              tr("Veriler yüklenirken hata oluştu: %1").arg(depotModel->lastError().text()));
        return;
    }

    ui->depotTableView->setModel(depotModel);

    // Modern tablo görünümü
    ui->depotTableView->setShowGrid(false);
    ui->depotTableView->setAlternatingRowColors(true);
    ui->depotTableView->verticalHeader()->setVisible(false);
    ui->depotTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->depotTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->depotTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->depotTableView->horizontalHeader()->setStretchLastSection(true);
    ui->depotTableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Sütun genişlikleri
    ui->depotTableView->setColumnWidth(0, 80);   // ID
    ui->depotTableView->setColumnWidth(1, 150);  // Depo Adı
    ui->depotTableView->setColumnWidth(2, 100);  // Şube
    ui->depotTableView->setColumnWidth(3, 150);  // Yönetici
    ui->depotTableView->setColumnWidth(4, ui->depotTableView->width() - 480); // Kapasite

    // Header stili
    QString headerStyle = "QHeaderView::section {"
                          "background-color: #1a237e;"
                          "color: white;"
                          "padding: 6px;"
                          "border: none;"
                          "border-bottom: 2px solid #3F51B5;"
                          "font-weight: bold;"
                          "}";

    // Tablo stili
    QString tableStyle = "QTableView {"
                         "background-color: #1e1e2f;"
                         "alternate-background-color: #2a2a45;"
                         "color: white;"
                         "selection-background-color: #3F51B5;"
                         "selection-color: white;"
                         "border: none;"
                         "}"
                         "QTableView::item {"
                         "padding: 5px;"
                         "border-bottom: 1px solid #34344a;"
                         "}"
                         "QTableView::item:selected {"
                         "background-color: #3F51B5;"
                         "}";

    // Scrollbar stili
    QString scrollbarStyle = "QScrollBar:vertical {"
                             "background-color: #1e1e2f;"
                             "width: 10px;"
                             "margin: 0px;"
                             "}"
                             "QScrollBar::handle:vertical {"
                             "background-color: #3F51B5;"
                             "min-height: 30px;"
                             "border-radius: 5px;"
                             "}"
                             "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
                             "height: 0px;"
                             "}";

    ui->depotTableView->horizontalHeader()->setStyleSheet(headerStyle);
    ui->depotTableView->setStyleSheet(tableStyle + scrollbarStyle);
}

void ReportsPage::exportDepotToExcel()
{
    QString fileName = getSaveFilePath("Excel Files (*.csv)");
    if (!fileName.isEmpty()) {
        exportToExcel(ui->depotTableView, fileName);
    }
}

void ReportsPage::exportDepotToPDF()
{
    QString fileName = getSaveFilePath("PDF Files (*.pdf)");
    if (!fileName.isEmpty()) {
        exportToPDF(ui->depotTableView, fileName);
    }
}

// Finansal Rapor Fonksiyonları
void ReportsPage::generateFinancialReport()
{
    if (!ui->financeGraphicsView->scene()) {
        ui->financeGraphicsView->setScene(new QGraphicsScene(this));
    }
    ui->financeGraphicsView->scene()->clear();

    QDate startDate = ui->financeStartDate->date();
    QDate endDate = ui->financeEndDate->date();
    QString reportType = ui->financeTypeCombo->currentText();

    QSqlQuery query(dbManager.getDatabase());
    QString queryStr;

    if (reportType == "Gelir Raporu") {
        queryStr = "SELECT Category, SUM(Amount) as Total FROM Transactions "
                   "WHERE TransactionType = 'Gelir' AND TransactionDate BETWEEN :startDate AND :endDate "
                   "GROUP BY Category";
    }
    else if (reportType == "Gider Raporu") {
        queryStr = "SELECT Category, SUM(Amount) as Total FROM Transactions "
                   "WHERE TransactionType = 'Gider' AND TransactionDate BETWEEN :startDate AND :endDate "
                   "GROUP BY Category";
    }
    else {
        queryStr = "SELECT Category, "
                   "SUM(CASE WHEN TransactionType = 'Gelir' THEN Amount ELSE -Amount END) as Total "
                   "FROM Transactions "
                   "WHERE TransactionDate BETWEEN :startDate AND :endDate "
                   "GROUP BY Category";
    }

    query.prepare(queryStr);
    query.bindValue(":startDate", startDate.toString("yyyy-MM-dd"));
    query.bindValue(":endDate", endDate.toString("yyyy-MM-dd"));

    if (!query.exec()) {
        QMessageBox::critical(this, tr("Hata"), tr("Sorgu çalıştırılırken hata oluştu!"));
        return;
    }

    QGraphicsScene* scene = ui->financeGraphicsView->scene();
    const int maxHeight = 200;
    const int barWidth = 50;
    const int spacing = 30;
    int xPos = 50;
    int yPos = maxHeight + 50;

    // Maksimum değeri bul
    double maxValue = 0;
    while (query.next()) {
        double value = qAbs(query.value("Total").toDouble());
        if (value > maxValue) maxValue = value;
    }

    // Grid çizgileri ve Y ekseni değerleri
    scene->addLine(xPos, 50, xPos, yPos, QPen(Qt::white));
    for (int i = 0; i <= 5; i++) {
        int y = yPos - (i * maxHeight / 5);
        // Grid çizgisi
        scene->addLine(xPos - 5, y, xPos + 600, y,
                       QPen(QColor(255, 255, 255, 30)));
        // Y ekseni değeri
        QGraphicsTextItem* valueText = scene->addText(
            QString::number(maxValue * i / 5, 'f', 2));
        valueText->setDefaultTextColor(Qt::white);
        valueText->setPos(5, y - 10);
    }

    // X ekseni
    scene->addLine(xPos, yPos, xPos + 600, yPos, QPen(Qt::white));

    // Sorguyu başa sar
    query.first();
    query.previous();

    // Grafik çizimi
    while (query.next()) {
        QString category = query.value("Category").toString();
        double value = query.value("Total").toDouble();

        int barHeight = maxValue > 0 ? qAbs(value) * maxHeight / maxValue : 0;

        // Bar rengi seç
        QColor barColor;
        if (reportType == "Kar/Zarar Raporu") {
            barColor = value >= 0 ? QColor("#2ecc71") : QColor("#e74c3c");
        } else {
            barColor = QColor("#2ecc71");
            if (reportType == "Gider Raporu") {
                barColor = QColor("#e74c3c");
            }
        }

        // Bar çiz
        QRectF barRect(xPos, value >= 0 ? yPos - barHeight : yPos,
                       barWidth, barHeight);
        scene->addRect(barRect, QPen(Qt::white), QBrush(barColor));

        // Değeri yaz
        QString valueStr = QString::number(qAbs(value), 'f', 2);
        QGraphicsTextItem* valueText = scene->addText(valueStr);
        valueText->setDefaultTextColor(Qt::white);
        valueText->setFont(QFont("Arial", 9, QFont::Bold));
        valueText->setPos(xPos + (barWidth - valueText->boundingRect().width()) / 2,
                          yPos - barHeight - 20);

        // Kategori adını yaz
        QGraphicsTextItem* categoryText = scene->addText(category);
        categoryText->setDefaultTextColor(Qt::white);
        categoryText->setFont(QFont("Arial", 9));
        categoryText->setRotation(-45);
        categoryText->setPos(xPos, yPos + 10);

        xPos += barWidth + spacing;
    }

    // View'ı güncelle
    ui->financeGraphicsView->setSceneRect(scene->itemsBoundingRect());
    ui->financeGraphicsView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);

    // Tabloyu güncelle
    updateFinancialTable();
}

void ReportsPage::updateFinancialTable()
{
    if (!financialModel) {
        financialModel = new QSqlTableModel(this, dbManager.getDatabase());
    }

    QDate startDate = ui->financeStartDate->date();
    QDate endDate = ui->financeEndDate->date();
    QString reportType = ui->financeTypeCombo->currentText();

    financialModel->setTable("Transactions");

    // Filtreleme
    QString filter = QString("TransactionDate BETWEEN '%1' AND '%2'")
                         .arg(startDate.toString("yyyy-MM-dd"))
                         .arg(endDate.toString("yyyy-MM-dd"));

    if (reportType != "Kar/Zarar Raporu") {
        QString transactionType = (reportType == "Gelir Raporu") ? "Gelir" : "Gider";
        filter += QString(" AND TransactionType = '%1'").arg(transactionType);
    }

    financialModel->setFilter(filter);

    // Başlıkları ayarla
    financialModel->setHeaderData(0, Qt::Horizontal, tr("İşlem No"));
    financialModel->setHeaderData(1, Qt::Horizontal, tr("Tarih"));
    financialModel->setHeaderData(2, Qt::Horizontal, tr("İşlem Tipi"));
    financialModel->setHeaderData(3, Qt::Horizontal, tr("Açıklama"));
    financialModel->setHeaderData(4, Qt::Horizontal, tr("Tutar"));
    financialModel->setHeaderData(5, Qt::Horizontal, tr("Kategori"));

    if (!financialModel->select()) {
        QMessageBox::critical(this, tr("Hata"),
                              tr("Veriler yüklenirken hata oluştu: %1").arg(financialModel->lastError().text()));
        return;
    }

    ui->financeTableView->setModel(financialModel);

    // Modern görünüm ayarları
    ui->financeTableView->setStyleSheet(
        "QTableView {"
        "    background-color: #1e1e2f;"
        "    alternate-background-color: #252540;"
        "    color: #ecf0f1;"
        "    border: none;"
        "    border-radius: 10px;"
        "    gridline-color: transparent;"
        "}"
        "QHeaderView::section {"
        "    background-color: #1a237e;"
        "    padding: 8px;"
        "    border: none;"
        "    color: white;"
        "    font-weight: bold;"
        "    border-bottom: 2px solid #3F51B5;"
        "}"
        "QTableView::item {"
        "    border: none;"
        "    border-bottom: 1px solid #34344a;"
        "    padding: 5px;"
        "}"
        "QTableView::item:selected {"
        "    background: #3F51B5;"
        "    color: white;"
        "}"
        "QScrollBar:vertical {"
        "    background-color: #1e1e2f;"
        "    width: 10px;"
        "    border-radius: 5px;"
        "}"
        "QScrollBar::handle:vertical {"
        "    background-color: #3F51B5;"
        "    min-height: 30px;"
        "    border-radius: 5px;"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "    height: 0px;"
        "}"
        );

    // Tablo ayarları
    ui->financeTableView->setShowGrid(false);
    ui->financeTableView->setAlternatingRowColors(true);
    ui->financeTableView->verticalHeader()->setVisible(false);
    ui->financeTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->financeTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->financeTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Sütun genişlikleri
    ui->financeTableView->setColumnWidth(0, 80);  // İşlem No
    ui->financeTableView->setColumnWidth(1, 120); // Tarih
    ui->financeTableView->setColumnWidth(2, 100); // İşlem Tipi
    ui->financeTableView->setColumnWidth(3, 250); // Açıklama
    ui->financeTableView->setColumnWidth(4, 100); // Tutar
    ui->financeTableView->setColumnWidth(5, ui->financeTableView->width() - 650); // Kategori

    // Satırları gelir/gider durumuna göre renklendir
    for (int row = 0; row < financialModel->rowCount(); ++row) {
        QString type = financialModel->data(financialModel->index(row, 2)).toString();
        for (int col = 0; col < financialModel->columnCount(); ++col) {
            QModelIndex index = financialModel->index(row, col);
            if (type == "Gelir") {
                ui->financeTableView->model()->setData(index, QColor("#2ecc71"), Qt::ForegroundRole);
            } else {
                ui->financeTableView->model()->setData(index, QColor("#e74c3c"), Qt::ForegroundRole);
            }
        }
    }
}

void ReportsPage::exportFinancialToExcel()
{
    QString fileName = getSaveFilePath("Excel Files (*.csv)");
    if (!fileName.isEmpty()) {
        exportToExcel(ui->financeTableView, fileName);
    }
}

void ReportsPage::exportFinancialToPDF()
{
    QString fileName = getSaveFilePath("PDF Files (*.pdf)");
    if (!fileName.isEmpty()) {
        exportToPDF(ui->financeTableView, fileName);
    }
}

void ReportsPage::onFinanceTypeChanged(const QString& type)
{
    generateFinancialReport();
}

// Yardımcı Fonksiyonlar
void ReportsPage::exportToExcel(QTableView* table, const QString& filename)
{
    QSqlTableModel* model = qobject_cast<QSqlTableModel*>(table->model());
    if (!model) {
        QMessageBox::critical(this, tr("Hata"), tr("Tablo verisi bulunamadı!"));
        return;
    }

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, tr("Hata"), tr("Dosya oluşturulamadı!"));
        return;
    }

    // BOM ekle
    file.write("\xEF\xBB\xBF");

    QTextStream stream(&file);

    // Başlıkları yaz
    QStringList headers;
    for (int col = 0; col < model->columnCount(); ++col) {
        headers << "\"" + model->headerData(col, Qt::Horizontal).toString() + "\"";
    }
    stream << headers.join(";") << Qt::endl;

    // Verileri yaz
    for (int row = 0; row < model->rowCount(); ++row) {
        QStringList rowData;
        for (int col = 0; col < model->columnCount(); ++col) {
            rowData << "\"" + model->data(model->index(row, col)).toString() + "\"";
        }
        stream << rowData.join(";") << Qt::endl;
    }

    file.close();
    QMessageBox::information(this, tr("Başarılı"), tr("CSV dosyası oluşturuldu."));
}

void ReportsPage::exportToPDF(QTableView* table, const QString& filename)
{
    QSqlTableModel* model = qobject_cast<QSqlTableModel*>(table->model());
    if (!model) {
        QMessageBox::critical(this, tr("Hata"), tr("Tablo verisi bulunamadı!"));
        return;
    }

    QString html;
    html += "<html><head><style>";
    // Tablo genişliğini %100'e çıkar ve daha geniş bir görünüm sağla
    html += "table { width: 98%; border-collapse: collapse; margin: 20px auto; }";
    // Hücre boyutlarını artır
    html += "th, td { border: 1px solid black; padding: 10px 12px; text-align: center; height: 50px; font-size: 12pt; }";
    html += "th { background-color: #f2f2f2; font-weight: bold; }";
    // Başlık boyutunu artır
    html += "h1 { text-align: center; font-size: 18pt; margin: 15px 0; }";
    html += "p { text-align: right; font-size: 11pt; margin: 10px 0; }";
    html += "</style></head><body>";

    html += "<h1>Stok Raporu</h1>";
    html += "<p>Tarih: " + QDate::currentDate().toString("dd.MM.yyyy") + "</p>";

    // Tablo başlıkları
    html += "<table><tr>";
    for (int col = 0; col < model->columnCount(); ++col) {
        html += "<th>" + model->headerData(col, Qt::Horizontal).toString() + "</th>";
    }
    html += "</tr>";

    // Tablo satırları
    for (int row = 0; row < model->rowCount(); ++row) {
        html += "<tr>";
        for (int col = 0; col < model->columnCount(); ++col) {
            html += "<td>" + model->data(model->index(row, col)).toString() + "</td>";
        }
        html += "</tr>";
    }
    html += "</table></body></html>";

    // PDF belgesi oluştur ve yazdır
    QPdfWriter pdfWriter(filename);
    pdfWriter.setPageSize(QPageSize(QPageSize::A4));
    // Kenar boşluklarını azalt
    pdfWriter.setPageMargins(QMarginsF(15, 15, 15, 15));
    // Yüksek çözünürlük
    pdfWriter.setResolution(300);

    QTextDocument document;
    document.setHtml(html);
    // Sayfa boyutunu ayarla
    document.setPageSize(QSizeF(pdfWriter.width(), pdfWriter.height()));
    document.print(&pdfWriter);

    QMessageBox::information(this, tr("Başarılı"), tr("PDF dosyası oluşturuldu."));
}


QString ReportsPage::getSaveFilePath(const QString& filter)
{
    return QFileDialog::getSaveFileName(this, tr("Dosyayı Kaydet"), QDir::homePath(), filter);
}
