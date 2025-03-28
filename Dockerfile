FROM ubuntu:20.04

# Timezone ayarı (apt-get komutları için)
ENV TZ=Europe/Istanbul
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

# Gerekli paketleri yükle
RUN apt-get update && apt-get install -y \
    build-essential \
    openmpi-bin \
    libopenmpi-dev \
    openssh-server \
    python3 \
    --no-install-recommends \
    && rm -rf /var/lib/apt/lists/*

# SSH konfigürasyonu
RUN mkdir /var/run/sshd
RUN echo 'root:password' | chpasswd
RUN sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config
RUN sed -i 's/#PasswordAuthentication yes/PasswordAuthentication yes/' /etc/ssh/sshd_config

# SSH anahtarlarını oluştur
RUN ssh-keygen -A

# MPI için SSH anahtarını oluştur
RUN mkdir -p /root/.ssh
RUN ssh-keygen -t rsa -f /root/.ssh/id_rsa -N ""
RUN cat /root/.ssh/id_rsa.pub >> /root/.ssh/authorized_keys
RUN echo "Host *\n\tStrictHostKeyChecking no\n\tUserKnownHostsFile /dev/null" > /root/.ssh/config

# Çalışma dizini oluştur
WORKDIR /app

# Çalışma kodu
COPY . /app/

# Verileri oluştur
RUN python3 generate_data.py

# Kodu derle
RUN make

# SSH servisi başlat
CMD ["/usr/sbin/sshd", "-D"]