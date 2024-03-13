FROM ubuntu:20.04

ENV LC_ALL=C.UTF-8
ARG DEBIAN_FRONTEND=noninteractive

ARG TARGETARCH
RUN if [ "$TARGETARCH" = "arm64" ]; then \
    dpkg --add-architecture amd64 \
    && grep ^deb /etc/apt/sources.list | sed 's/deb /deb [arch=arm64] /' | tee /tmp/sources.list \
    && grep ^deb /etc/apt/sources.list | sed 's/deb /deb [arch=amd64,i386] /g; s/ports\.ubuntu/archive.ubuntu/g; s/ubuntu-ports/ubuntu/g'  | tee -a /tmp/sources.list \
    && mv /tmp/sources.list /etc/apt/sources.list; \
    fi

RUN apt-get update \
    && apt-get install -y sudo wget software-properties-common libc6:amd64 libstdc++6:amd64

RUN wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add - \
    && add-apt-repository 'deb http://apt.llvm.org/focal/ llvm-toolchain-focal-16 main' \
    && apt-get install -y clangd-16 clang-format-16 clang-tidy-16

RUN update-alternatives --install /usr/bin/clangd clangd /usr/bin/clangd-16 10 \
    && update-alternatives --install /usr/bin/clang-format clang-format /usr/bin/clang-format-16 10 \
    && update-alternatives --install /usr/bin/clang-tidy clang-tidy /usr/bin/clang-tidy-16 10

RUN apt-get install -y git make gcc kmod flex bison libssl-dev bc gawk rsync xxd unzip file xz-utils zlib1g-dev ninja-build python3 python-is-python3

RUN dpkg --add-architecture i386 \
    && apt-get update \
    && apt-get install -y libc6:i386 libstdc++6:i386 zlib1g:i386 libglib2.0-0:i386

RUN apt-get autoremove -y \
    && apt-get clean -y \
    && rm -rf /var/lib/apt/lists/*

RUN echo "dash dash/sh boolean false" | debconf-set-selections \
    && dpkg-reconfigure dash

ARG USERNAME=vscode
ARG USER_UID=1000
ARG USER_GID=$USER_UID

RUN groupadd --gid $USER_GID $USERNAME \
    && useradd --uid $USER_UID --gid $USER_GID -m $USERNAME \
    && echo $USERNAME ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/$USERNAME \
    && chmod 0440 /etc/sudoers.d/$USERNAME

RUN echo $'if [ -x /usr/bin/id ]; then\n\
    USER="`/usr/bin/id -un`"\n\
    LOGNAME=$USER\n\
fi\n\
export USER LOGNAME' >> /etc/profile.d/logname-fix.sh

RUN mkdir -p /opt/toolchains \
    && wget --progress=dot:mega -O- https://github.com/Broadcom/stbgcc-4.5/releases/download/stbgcc-4.5.4-2.9/stbgcc-4.5.4-2.9.tar.bz2 | tar -jxf - -C /opt/toolchains/ \
    && wget --progress=dot:mega -O- https://github.com/Broadcom/stbgcc-4.8/releases/download/stbgcc-4.8-1.6/stbgcc-4.8-1.6.tar.bz2 | tar -jxf - -C /opt/toolchains/

RUN ln -s /tmp /nfsroot && ln -s /tmp /tftpboot
