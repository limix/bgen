FROM ubuntu:rolling
RUN apt-get update && apt-get upgrade --yes
RUN apt-get install build-essential curl zlib1g-dev cmake gdb valgrind --yes
RUN bash <(curl -fsSL https://raw.githubusercontent.com/horta/almosthere/master/install)
RUN bash <(curl -fsSL https://raw.githubusercontent.com/horta/zstd.install/master/install)
ENTRYPOINT ["/bin/bash"]
