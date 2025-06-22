FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    qtbase5-dev \
    qt5-qmake \
    libqt5network5 \
    libqt5sql5 \
    cmake \
    build-essential \
    git \
    libssl-dev \
    zlib1g-dev \
    && rm -rf /var/lib/apt/lists/*

# Устанавливаем рабочую директорию
WORKDIR /app

# Копируем файлы проекта
COPY . .

RUN cd Server && \
    qmake Server.pro && \
    make

# Сборка клиента
RUN cd client && \
    qmake client.pro && \
    make



CMD ["/app/Server/Server"]