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

# Separate C++ learning workspace (source files only)
COPY docker-cpp-workspace/ /opt/cpp-workspace/
RUN mkdir -p /opt/cpp-workspace/build /opt/cpp-workspace/custom

WORKDIR /opt/ns-3.48/simulator-gui/web

ENV NS3_PROJECT_ROOT=/opt/ns-3.48
ENV CPP_WORKSPACE_DIR=/opt/cpp-workspace
ENV PORT=10000

EXPOSE 10000

CMD ["sh", "-c", "gunicorn --bind 0.0.0.0:${PORT} --workers 1 --threads 4 --timeout 120 --access-logfile - --error-logfile - app:app"]
