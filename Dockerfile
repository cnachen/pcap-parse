FROM artifactory.momenta.works/docker/ubuntu:22.04
LABEL maintainer="ningan.chen <ningan.chen@momenta.ai>"

WORKDIR /work

ENV TZ=Asia/Shanghai \
    DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get upgrade
RUN apt-get install -y build-essential cmake ninja-build pkg-config git neovim tmux
RUN apt-get install -y libpcap-dev libjsoncpp-dev libssl-dev

RUN apt-get install -y tzdata
RUN ln -sf /usr/share/zoneinfo/Asia/Shanghai /etc/localtime

RUN apt-get clean
RUN rm -rf /var/lib/apt/lists/*

COPY ./ /work/buildroot

RUN make -C /work/buildroot && make -C /work/buildroot install

CMD ["/bin/bip-eck"]