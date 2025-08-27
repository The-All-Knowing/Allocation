# Базовый образ
FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
  build-essential \
  g++ \
  make \
  cmake \
  python3 \
  python3-pip \
  git \
  sudo \
  && \
  pip install --no-cache-dir conan && \
  # Очищаем кэш пакетов apt для минимизации размера образа
  rm -rf /var/lib/apt/lists/*

CMD ["/bin/bash"]