FROM ubuntu:18.04

RUN apt-get update -qq && apt-get install --no-install-recommends -y --force-yes curl apache2 apache2-utils openssl ca-certificates nodejs make cmake build-essential default-jre python python-minimal python3.6 git ruby && apt-get clean && rm -rf /var/lib/apt/lists/*

RUN useradd --home-dir /home/app --create-home --shell /bin/bash app

RUN cd /opt && git clone https://github.com/juj/emsdk.git && cd emsdk && git pull && ./emsdk list && ./emsdk install 1.39.19 && chown -R app /opt/emsdk && cd /opt/emsdk && ./emsdk activate 1.39.19 

RUN a2enmod headers rewrite
RUN a2dissite 000-default

ENV APACHE_RUN_USER app
ENV APACHE_RUN_GROUP app
ENV APACHE_LOG_DIR /var/log/apache2
ENV APACHE_PID_FILE /var/run/apache2/pid
ENV APACHE_LOCK_DIR /var/lock/apache2
ENV APACHE_RUN_DIR /var/run/apache2

RUN htpasswd -cb /etc/apache2/glsl-qwqz.password guest guest
RUN chown root:app /etc/apache2/glsl-qwqz.password
RUN chmod 640 /etc/apache2/glsl-qwqz.password
RUN echo "Listen 8080" | tee /etc/apache2/ports.conf

RUN mkdir -p /var/lock/apache2 /var/www/html; chown app /var/log/apache2 /var/lock/apache2 /var/www/html

RUN update-alternatives --install /usr/bin/python python /usr/bin/python3.6 10

COPY [".", "/home/app"]

RUN chown -R app /home/app

USER app
RUN cd /opt/emsdk && . /opt/emsdk/emsdk_env.sh && cd /home/app && make clean
RUN cd /opt/emsdk && . /opt/emsdk/emsdk_env.sh && cd /home/app && make emscripten-sidescroll
RUN cd /opt/emsdk && . /opt/emsdk/emsdk_env.sh && cd /home/app && make emscripten-rocketeer
RUN cd /opt/emsdk && . /opt/emsdk/emsdk_env.sh && cd /home/app && make emscripten-ntm
RUN cd /opt/emsdk && . /opt/emsdk/emsdk_env.sh && cd /home/app && make emscripten-test

USER root
ADD glsl-qwqz.conf /etc/apache2/sites-available/glsl-qwqz.conf
RUN a2ensite glsl-qwqz
RUN rm /var/www/html/index.html

RUN mkdir -p /var/lock/apache2 /var/www/html; chown -Rv app /var/log/apache2 /var/lock/apache2 /var/www/html

USER app
WORKDIR /home/app

#CMD ["apache2", "-t"]
#RUN mkdir -p /var/incoming; chown www-data /var/incoming
#USER app
#USER app
