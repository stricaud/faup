FROM debian:bullseye
RUN apt-get update
RUN apt-get install -y emacs-nox vim jed unzip zip
RUN apt-get install -y wget git make cmake gcc g++
RUN apt-get install -y liblua5.3-dev

RUN useradd -ms /bin/bash faupuser
USER faupuser
WORKDIR /home/faupuser/
RUN git clone https://github.com/stricaud/tutoprompt/
RUN mkdir /home/faupuser/.tutoprompt/
RUN cp tutoprompt/examples/faupworkshop/* /home/faupuser/.tutoprompt/
RUN echo ". ./tutoprompt/tutoprompt.sh" >> /home/faupuser/.bashrc

RUN git clone https://github.com/stricaud/faup
WORKDIR /home/faupuser/faup/build
RUN cmake ..
RUN make

USER root
WORKDIR /home/faupuser/faup/build
RUN make install
RUN ldconfig
RUN mkdir /opt/faup/
RUN mv /usr/local/bin/faup /opt/faup/
COPY faup-wrapper /usr/local/bin/faup

USER faupuser
WORKDIR /home/faupuser/
RUN wget http://s3.amazonaws.com/alexa-static/top-1m.csv.zip
