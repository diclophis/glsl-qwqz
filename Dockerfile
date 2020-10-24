FROM ubuntu:16.10

RUN apt-get update -qq && apt-get install --no-install-recommends -y --force-yes curl openssl ca-certificates nodejs make cmake build-essential default-jre python python-minimal python2.7 && apt-get clean && rm -rf /var/lib/apt/lists/*

RUN useradd --home-dir /home/app --create-home --shell /bin/bash app

USER app
WORKDIR /home/app

RUN curl "https://s3.amazonaws.com/mozilla-games/emscripten/releases/emsdk-portable.tar.gz" -o /var/tmp/emsdk-portable.tar.gz
RUN tar zxvf /var/tmp/emsdk-portable.tar.gz

RUN emsdk-portable/emsdk update
RUN emsdk-portable/emsdk install latest
RUN emsdk-portable/emsdk activate latest

COPY [".", "/home/app"]

#Setting environment variables:
#EMSDK = /emsdk-portable
#EM_CONFIG = /home/app/.emscripten

RUN make emscripten-test

#
#WORKDIR /home/app
#RUN chown -R app. /home/app
#USER app
#RUN bundle install --force --path=vendor/bundle --jobs=4 --retry=3 --deployment
#
#COPY [".bash_profile", "/home/app/"]
#COPY ["bespoked.config.ru", "puma.config.ru", "Rakefile", "/home/app/"]
#COPY ["app", "/home/app/app"]
#COPY ["bin", "/home/app/bin"]
#COPY ["config", "/home/app/config"]
#COPY ["db", "/home/app/db"]
#COPY ["lib", "/home/app/lib"]
#COPY ["public", "/home/app/public"]
#
#USER root
#WORKDIR /home/app
#RUN chown -R app. /home/app
#USER app
#
#RUN mkdir -p /home/app/public/assets
#RUN bundle exec rake assets:precompile
#
#CMD ["bundle", "exec", "rackup", "-h"]
