FROM ubuntu:22.04

# Устанавливаем часовой пояс и отключаем интерактивные запросы
ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Etc/UTC

# Устанавливаем необходимые зависимости
RUN apt-get update && apt-get install -y \
    build-essential \
    g++ \
    cmake \
    make \
    python3 \
    python3-pip \
    python3-venv \
    git \
    python3-autopep8 \
    pipx \
    sudo \
    tzdata \
    && pipx ensurepath \
    && pipx install conan \
    && rm -rf /var/lib/apt/lists/*

# Добавляем pipx в PATH
ENV PATH="/root/.local/bin:${PATH}"

# Команда по умолчанию
CMD ["/bin/bash"]