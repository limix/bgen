FROM ubuntu:rolling
RUN apt-get update && apt-get upgrade --yes
RUN apt-get install build-essential curl zlib1g-dev cmake gdb valgrind --yes
RUN curl https://raw.githubusercontent.com/horta/almosthere/master/install --output install-almosthere
RUN curl https://raw.githubusercontent.com/horta/zstd.install/master/install --output install-zstd
RUN /bin/bash install-almosthere
RUN /bin/bash install-zstd
CMD ["bash"]
