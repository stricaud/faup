FROM debian:bullseye
RUN apt-get update
RUN apt-get install -y emacs-nox vim jed unzip zip
RUN apt-get install -y wget git make cmake gcc g++
RUN apt-get install -y liblua5.3-dev

RUN useradd -ms /bin/bash faupuser
USER faupuser
WORKDIR /home/faupuser/
RUN git clone https://github.com/stricaud/faup
WORKDIR /home/faupuser/faup/build
RUN cmake ..
RUN make

USER root
WORKDIR /home/faupuser/faup/build
RUN make install
RUN ldconfig

USER faupuser
WORKDIR /home/faupuser/
RUN wget http://s3.amazonaws.com/alexa-static/top-1m.csv.zip
