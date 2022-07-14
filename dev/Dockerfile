# WIP

FROM ubuntu:22.04

RUN apt update \
 && apt install -y build-essential \
  libboost-fiber-dev libboost-context-dev libmsgpack-dev \
  libxcb1-dev libx11-dev libx11-xcb-dev libgl-dev libxcb-xinput-dev \
  git python3 cmake

RUN cd /opt && git clone https://github.com/emscripten-core/emsdk.git

RUN cd /opt/emsdk \
 && ./emsdk update \
 && ./emsdk install latest \
 && ./emsdk activate latest
