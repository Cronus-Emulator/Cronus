FROM ubuntu:14.04

RUN useradd -ms /bin/bash cronus

WORKDIR /home/cronus

RUN apt-get update && apt-get install -y gcc make libmysqlclient-dev zlib1g-dev libpcre3-dev

COPY . /home/cronus

RUN chown -R cronus /home/cronus
USER cronus

RUN sh configure
RUN make clean && make sql

EXPOSE 5121
EXPOSE 6121
EXPOSE 6900

ENTRYPOINT ["./docker-entrypoint.sh"]
