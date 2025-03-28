MPI ve OpenMP Performans Karşılaştırması

Bu projede, Docker konteynerlerinde simüle edilen bir dağıtık sistem üzerinde MPI ve OpenMP'nin performansını inceledik.

Test Ortamı

Docker Compose ile oluşturulan 4 konteynerli sistem
Ana veri seti: 10000 satır (her satırda 10 rastgele sayı)
İşlem: Sayıları toplama ve CPU yoğun matematik işlemleri
MPI ve OpenMP Karşılaştırması:

MPI (Message Passing Interface):

Mimari: Dağıtık bellek mimarisi
Avantajlar:
Birden fazla fiziksel makine/konteyner arasında çalışabilme
İyi ölçeklenebilirlik (düğüm sayısı arttıkça performans artışı)
Büyük veri setleri için uygun
Dezavantajlar:
İletişim maliyeti var (düğümler arası veri paylaşımı)
Uygulama karmaşıklığı yüksek
OpenMP (Open Multi-Processing):

Mimari: Paylaşımlı bellek mimarisi
Avantajlar:
Kolay uygulama (pragma direktifleri)
Düşük iletişim maliyeti (aynı bellek alanı kullanılıyor)
Hızlı geliştirme imkanı
Dezavantajlar:
Tek makine/konteyner ile sınırlı
Thread sayısı arttıkça ek maliyet artışı
Bellek sınırlamaları
Hibrit Yaklaşım (MPI + OpenMP):

Projemizde MPI ve OpenMP'nin birlikte kullanımı en iyi performansı sağladı
4 düğüm ve 20 thread ile tek düğüm tek thread'e göre yaklaşık 16x hızlanma
Bu, her iki teknolojinin de avantajlarını birleştirdiğimizi gösteriyor
