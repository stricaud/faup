FROM debian:bullseye
MAINTAINER Sebastien Tricaud <sebastien@honeynet.org>
RUN apt update -q
Run apt -qy install cron git cmake liblua5.3-dev jed g++
COPY faup-test.cron /etc/cron.d/faup-test
RUN chmod ugo+r /etc/cron.d/faup-test
RUN crontab /etc/cron.d/faup-test
WORKDIR /root
RUN git clone http://github.com/stricaud/faup /root/faup
WORKDIR /root/faup/build
RUN cmake ..
RUN make
RUN make install
RUN ldconfig
COPY runfaup.sh /root
RUN chmod ugo+x /root/runfaup.sh
CMD ["cron", "-f"]
