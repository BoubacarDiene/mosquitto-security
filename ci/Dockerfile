##
# docker build -t  mosquitto-image ci/
# docker run --privileged -it -u $(id -u) --rm -v $(pwd):/workdir mosquitto-image:latest
##

# Download base image ubuntu 18.04
FROM ubuntu:18.04

# Define labels (maintainer and description)
LABEL maintainer="Boubacar DIENE <boubacar.diene@gmail.com>"
LABEL description="Docker image that contains everything necessary to work with mosquitto"

# Install required packages from ubuntu repository
RUN apt-get update && apt-get install -y --no-install-recommends \
        build-essential \
        cppcheck \
        ctags \
        doxygen \
        gdb \
        git \
        graphviz \
        libprotobuf-dev \
        libssl-dev \
        net-tools \
        openssl \
        protobuf-compiler \
        strace \
        sudo \
        tree \
        uthash-dev \
        valgrind \
        vim \
        wget && \
    apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

# Environment variables
ENV MOSQUITTO_VERSION 1.6.9
ENV MOSQUITTO_USERNAME mosquitto-usr
ENV MOSQUITTO_HOMEDIR /home/${MOSQUITTO_USERNAME}

# Add user
# Password disabled because of "sudo xxx" commands
RUN adduser --disabled-password --gecos '' ${MOSQUITTO_USERNAME}
RUN adduser ${MOSQUITTO_USERNAME} sudo
RUN echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers

USER ${MOSQUITTO_USERNAME}
WORKDIR /workdir

# Install cmake
# Note: CMAKE_VERSION=x.y.z => ${CMAKE_VERSION%.*} = x.y (used below)
ARG CMAKE_VERSION=3.17.0
ARG CMAKE_BUILD_ID=rc3
ARG CMAKE_INSTALLER=cmake-${CMAKE_VERSION}-${CMAKE_BUILD_ID}-Linux-x86_64.sh

RUN wget --no-check-certificate https://cmake.org/files/v${CMAKE_VERSION%.*}/${CMAKE_INSTALLER} \
    -O ${MOSQUITTO_HOMEDIR}/${CMAKE_INSTALLER}
RUN cd ${MOSQUITTO_HOMEDIR} \
    && sudo sh ${CMAKE_INSTALLER} --skip-license --prefix=/opt \
    && sudo ln -s /opt/bin/cmake /usr/local/bin/cmake \
    && rm ${CMAKE_INSTALLER}

# Install Eclipse's version of mosquitto
RUN wget --no-check-certificate https://mosquitto.org/files/source/mosquitto-${MOSQUITTO_VERSION}.tar.gz -O - \
    | tar xz -C ${MOSQUITTO_HOMEDIR}
RUN cd ${MOSQUITTO_HOMEDIR}/mosquitto-${MOSQUITTO_VERSION} \
    && cmake . -DCMAKE_INSTALL_PREFIX=/usr \
    && make && sudo make install \
    && rm -rf ${MOSQUITTO_HOMEDIR}/mosquitto-${MOSQUITTO_VERSION}*

# Install vim plugins
ARG CTRLP_VIM_VERSION=1.79
ARG NERDTREE_VERSION=6.7.0
ARG VIM_OPERATOR_USER_VERSION=0.1.0

RUN mkdir -p ${MOSQUITTO_HOMEDIR}/.vim/autoload ${MOSQUITTO_HOMEDIR}/.vim/bundle \
    && wget --no-check-certificate https://tpo.pe/pathogen.vim -P ${MOSQUITTO_HOMEDIR}/.vim/autoload/ \
    && cd ${MOSQUITTO_HOMEDIR}/.vim/bundle \
    && mkdir ctrlp.vim \
        && wget --no-check-certificate https://github.com/kien/ctrlp.vim/archive/${CTRLP_VIM_VERSION}.tar.gz -O - \
        | tar xz --strip-components=1 -C ctrlp.vim \
    && mkdir nerdtree \
        && wget --no-check-certificate https://github.com/preservim/nerdtree/archive/${NERDTREE_VERSION}.tar.gz -O - \
        | tar xz --strip-components=1 -C nerdtree \
    && mkdir vim-operator-user \
        && wget --no-check-certificate https://github.com/kana/vim-operator-user/archive/${VIM_OPERATOR_USER_VERSION}.tar.gz -O - \
        | tar xz --strip-components=1 -C vim-operator-user

COPY .vimrc ${MOSQUITTO_HOMEDIR}/

# Entry point
ENTRYPOINT ["/bin/bash"]
