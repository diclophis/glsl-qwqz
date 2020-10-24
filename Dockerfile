FROM ubuntu:18.04

RUN apt-get update -qq && apt-get install --no-install-recommends -y --force-yes curl openssl ca-certificates nodejs make cmake build-essential default-jre python python-minimal python2.7 git ruby && apt-get clean && rm -rf /var/lib/apt/lists/*

RUN useradd --home-dir /home/app --create-home --shell /bin/bash app

RUN cd /opt && git clone https://github.com/juj/emsdk.git && cd emsdk && git pull && ./emsdk list && ./emsdk install 1.39.19 && chown -R app /opt/emsdk && cd /opt/emsdk && ./emsdk activate 1.39.19 

RUN apt-get update
RUN apt-get install -y apache2 apache2-utils

RUN a2enmod dav dav_fs headers rewrite
RUN a2dissite 000-default

ENV APACHE_RUN_USER app
ENV APACHE_RUN_GROUP app
ENV APACHE_LOG_DIR /var/log/apache2
ENV APACHE_PID_FILE /var/run/apache2.pid
ENV APACHE_LOCK_DIR /var/lock/apache2
ENV APACHE_RUN_DIR /var/run/apache2

RUN htpasswd -cb /etc/apache2/webdav.password guest guest
RUN chown root:app /etc/apache2/webdav.password
RUN chmod 640 /etc/apache2/webdav.password
RUN echo "Listen 8080" | tee /etc/apache2/ports.conf

RUN mkdir -p /var/lock/apache2 /var/www/html; chown app /var/log/apache2 /var/lock/apache2 /var/www/html

COPY [".", "/home/app"]

RUN chown -R app /home/app

USER app
RUN cd /opt/emsdk && . /opt/emsdk/emsdk_env.sh && cd /home/app && make emscripten-test

USER root
ADD glsl-qwqz.conf /etc/apache2/sites-available/glsl-qwqz.conf
RUN a2ensite glsl-qwqz

#CMD ["apache2", "-t"]
#RUN mkdir -p /var/incoming; chown www-data /var/incoming
#ADD index.html /var/www/html/index.html
#USER app
#USER app
#WORKDIR /home/app
