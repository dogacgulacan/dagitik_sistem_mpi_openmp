#!/usr/bin/env python3

import random
import os

# Veri dizinini oluştur
if not os.path.exists('data'):
    os.makedirs('data')

# Veri dosyasını oluştur
with open('data/input_data.txt', 'w') as f:
    # 10000 satır veri yaz
    for i in range(10000):
        # Her satıra 10 rastgele sayı ekle
        line = ' '.join(str(random.uniform(-100, 100)) for _ in range(10))
        f.write(line + '\n')

print("Veri dosyası oluşturuldu: data/input_data.txt")