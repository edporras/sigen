FROM buildpack-deps:stretch
MAINTAINER github@digressed.net

ENV LC_ALL C.UTF-8

# USER root
# RUN groupadd -r ned -g 1000 && \
#     useradd -u 1000 -r -g ned -m -s /sbin/nologin -c "Docker image user" ned && \
#     chown -R ned:ned /home/ned && \
#     mkdir /home/ned/bin

# WORKDIR /home/ned/app

USER ned

RUN autoreconf -i && ./configure && make -j 2

ENTRYPOINT ["docker/entrypoint"]
