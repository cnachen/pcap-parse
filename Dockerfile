FROM artifactory.momenta.works/docker/ubuntu:22.04
LABEL maintainer="ningan.chen <ningan.chen@momenta.ai>"

WORKDIR /work

ENV TZ=Asia/Shanghai \
    DEBIAN_FRONTEND=noninteractive

COPY ./ /work/build

RUN apt-get update && apt-get upgrade
RUN apt-get install -y build-essential cmake pkg-config git neovim tmux
RUN apt-get install -y libpcap-dev libjsoncpp-dev libssl-dev

RUN apt-get install -y tzdata
RUN ln -sf /usr/share/zoneinfo/Asia/Shanghai /etc/localtime

RUN apt-get clean
RUN rm -rf /var/lib/apt/lists/*

RUN make -C /work/build && make -C /work/build install

CMD ["/bin/bash"]