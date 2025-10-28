# 📦 Stok Envanter Yönetim Sistemi

Modern ve kullanıcı dostu bir **Stok Envanter Yönetim Sistemi**. Qt6 ve C++ ile geliştirilmiş masaüstü uygulaması.

![Qt](https://img.shields.io/badge/Qt-6.8.0-green.svg)
![C++](https://img.shields.io/badge/C++-17-blue.svg)
![License](https://img.shields.io/badge/license-MIT-orange.svg)

## 🚀 Özellikler

### 🔐 Kullanıcı Yönetimi
- Güvenli giriş sistemi
- Kullanıcı rolleri (Admin & Çalışan)
- Rol tabanlı yetkilendirme

### 📋 Ürün Yönetimi
- Ürün ekleme, güncelleme ve silme
- Ürün kategorilendirme
- Detaylı ürün bilgileri (kod, ad, kategori, fiyat, stok)
- Arama ve filtreleme

### 🏭 Depo Yönetimi
- Depo ekleme ve yönetimi
- Depo lokasyon takibi
- Depo doluluk analizi
- Grafik görünüm

### 🔄 Transfer İşlemleri
- Depolar arası ürün transferi
- Transfer geçmişi takibi
- Stok hareketleri

### 📊 Raporlama
- **Stok Raporu**: Anlık stok durumu
- **Depo Raporu**: Depo bazlı analiz
- **Finansal Rapor**: Gelir/gider analizi
- Excel export
- PDF export

## 🛠️ Teknolojiler

- **Framework**: Qt 6.8.0
- **Dil**: C++ 17
- **Veritabanı**: Microsoft SQL Server
- **Build Sistemi**: CMake
- **Compiler**: MinGW 64-bit

## 📋 Gereksinimler

### Yazılım Gereksinimleri
- Qt 6.8.0 veya üzeri
- MinGW 64-bit compiler
- Microsoft SQL Server (Express veya üzeri)
- CMake 3.29 veya üzeri

### SQL Server Gereksinimleri
- SQL Server instance'ı çalışır durumda olmalı
- `StokEnvanterDB` veritabanı oluşturulmuş olmalı
- Windows Authentication aktif olmalı

## 🚀 Kurulum

### 1. Projeyi Klonlayın
```bash
git clone https://github.com/[KULLANICI_ADI]/StokEnvanterQt.git
cd StokEnvanterQt
```

### 2. Veritabanını Ayarlayın
```sql
-- SQL Server'da StokEnvanterDB veritabanını oluşturun
CREATE DATABASE StokEnvanterDB;
GO

USE StokEnvanterDB;
GO

-- Gerekli tabloları oluşturun
-- Users, Products, Depots, Transfers, vb.
```

### 3. Bağlantı Ayarlarını Güncelleyin
`database_manager.cpp` dosyasında SQL Server bağlantı bilgilerinizi güncelleyin:

```cpp
db.setDatabaseName("DRIVER={SQL Server};SERVER=SUNUCU_ADI\\SQLEXPRESS;DATABASE=StokEnvanterDB;Trusted_Connection=yes;");
```

### 4. Projeyi Derleyin
Qt Creator ile:
```
1. StokEnvanterQt/CMakeLists.txt dosyasını açın
2. Build > Build All
3. Run
```

Komut satırından:
```bash
cd StokEnvanterQt
mkdir build && cd build
cmake ..
cmake --build .
./StokEnvanterQt.exe
```

## 📖 Kullanım

1. **Giriş Yapın**: Kullanıcı adı, şifre ve kullanıcı tipi seçerek giriş yapın
2. **Ana Menü**: Sol taraftaki menüden istediğiniz modülü seçin
3. **Ürün Yönetimi**: Ürünlerinizi ekleyin, düzenleyin veya silin
4. **Depo Yönetimi**: Depolarınızı organize edin
5. **Transfer**: Ürünlerinizi depolar arası transfer edin
6. **Raporlar**: Detaylı raporlar alın ve export edin

## 🗂️ Proje Yapısı

```
StokEnvanterQt/
├── CMakeLists.txt           # CMake yapılandırması
├── main.cpp                 # Ana giriş noktası
├── database_manager.*       # Veritabanı yönetimi
├── login_page.*             # Giriş sayfası
├── mainwindow.*             # Ana pencere
├── product_page.*           # Ürün yönetimi
├── depot_page.*             # Depo yönetimi
├── transfer_page.*          # Transfer işlemleri
├── reports_page.*           # Raporlama modülü
└── *.ui                     # Qt Designer arayüz dosyaları
```

## 🤝 Katkıda Bulunma

Katkılarınızı bekliyoruz! Lütfen şu adımları izleyin:

1. Projeyi fork edin
2. Feature branch oluşturun (`git checkout -b feature/AmazingFeature`)
3. Değişikliklerinizi commit edin (`git commit -m 'feat: Add some AmazingFeature'`)
4. Branch'inizi push edin (`git push origin feature/AmazingFeature`)
5. Pull Request açın

## 📝 Lisans

Bu proje MIT lisansı altında lisanslanmıştır. Detaylar için [LICENSE](LICENSE) dosyasına bakın.

## 👥 Katkıda Bulunanlar

Bu projeye katkıda bulunan herkese teşekkürler! 🎉

<!-- ALL-CONTRIBUTORS-LIST:START -->
<!-- ALL-CONTRIBUTORS-LIST:END -->

## 📧 İletişim

Sorularınız veya önerileriniz için issue açabilirsiniz.

## 🙏 Teşekkürler

- Qt Framework ekibine harika bir framework için
- Tüm katkıda bulunanlara

---

⭐ Bu projeyi beğendiyseniz yıldız vermeyi unutmayın!

