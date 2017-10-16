FROM        ubuntu:xenial
MAINTAINER  alex

# update and install dependencies
RUN         apt-get update \
                && apt-get install -y \
                    software-properties-common \
                    wget \
                && add-apt-repository -y ppa:ubuntu-toolchain-r/test \
                && apt-get update \
                && apt-get install -y \
                    make \
                    git \
                    curl \
                    vim \
                    vim-gnome \
                && apt-get install -y gcc-6\
                && update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-6 100\
                && apt-get install build-essential -y
RUN curl -O  https://cmake.org/files/v3.10/cmake-3.10.0-rc2.tar.gz\
    && tar -xvf cmake-3.10.0-rc2.tar.gz
WORKDIR cmake-3.10.0-rc2
RUN ./bootstrap \
    && make \
    && make install
WORKDIR ..

EXPOSE 80 80

COPY  httpd.conf /etc/httpd.conf
ADD . /home/ksg/disk_d/Mail/HightLoad/fst/
WORKDIR /home/ksg/disk_d/Mail/HightLoad/fst/
RUN cmake .
RUN make fst
ENTRYPOINT ./fst
# sudo docker run -v /home/ksg/disk_d/Mail/HightLoad/http-test-suite/:/www/static -p 80:80  fsttest
# sudo docker build -t fsttest .
# ab -n 100000 -c 100 127.0.0.1:80/httptest/dir2/page.html
# /home/ksg/disk_d/Mail/HightLoad/fst
# sudo docker run -v /home/ksg/disk_d/Mail/HightLoad/http-test-suite/:/www/static -v /home/ksg/disk_d/Mail/HightLoad/fst/httpd.conf:/etc/httpd.conf  -p 80:80  fsttest