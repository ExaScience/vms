FROM ompss_at_fpga:2.5.2-euroexa

USER root

RUN apt-get update && apt-get install -y \
  qemu-system-arm \
  gosu \
  && rm -rf /var/lib/apt/lists/*

ENV OMPSS_ROOT=/opt/bsc/x86_64/ompss/2.5.2-euroexa

ENV PATH=${PATH}:${OMPSS_ROOT}/mcxx-arm64/bin:${OMPSS_ROOT}/ait:${OMPSS_ROOT}/mcxx-x86_64/bin

COPY entrypoint.sh /dockfpga/euroexa_entrypoint.sh
COPY dockfpga /dockfpga/

ENTRYPOINT ["/dockfpga/euroexa_entrypoint.sh"]
