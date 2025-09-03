FROM ubuntu:latest

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
  build-essential \
  cmake \
  pipx \
  git \
  sudo \
  pycodestyle \
  clang-format \
  && \
  pipx ensurepath && \
  pipx install conan && \
  pipx install autopep8 && \
  rm -rf /var/lib/apt/lists/*

ENV PATH="/root/.local/bin:${PATH}"

CMD ["/bin/bash"]