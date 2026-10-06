FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    python3 \
    python3-pip \
    git \
    wget \
    ca-certificates \
    libsqlite3-dev \
    libeigen3-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /opt

RUN wget -q https://www.nsnam.org/releases/ns-3.48.tar.bz2 \
    && tar -xjf ns-3.48.tar.bz2 \
    && rm -f ns-3.48.tar.bz2

COPY 6g-lena /opt/ns-3.48/contrib/6g-lena

WORKDIR /opt/ns-3.48

RUN ./ns3 configure \
    --enable-examples \
    --enable-tests

RUN ./ns3 build

COPY requirements.txt /opt/requirements.txt

RUN pip3 install --break-system-packages -r /opt/requirements.txt

COPY simulator-gui/web /opt/ns-3.48/simulator-gui/web

WORKDIR /opt/ns-3.48/simulator-gui/web

ENV PORT=10000

EXPOSE 10000

CMD ["sh", "-c", "python3 app.py --host 0.0.0.0 --port ${PORT}"]
