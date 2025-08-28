FROM ubuntu:latest

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
  build-essential \
  cmake \
  pipx \
  git \
  sudo \
  pycodestyle \
  && \
  pipx ensurepath && \
  pipx install conan && \
  rm -rf /var/lib/apt/lists/*

ENV PATH="/root/.local/bin:${PATH}"

CMD ["/bin/bash"]