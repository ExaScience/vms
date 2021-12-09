FROM ompss_at_fpga:3.1.3-euroexa

USER root

RUN apt-get update && apt-get install -y \
  qemu-user \
  gosu \
  && rm -rf /var/lib/apt/lists/*

ENV OMPSS_ROOT=/opt/bsc/x86_64/ompss/3.1.3-euroexa
ENV XILINX_VITIS=/home/vanderaa/local/software/Xilinx/Vitis/2020.1
ENV XILINX_VIVADO=/home/vanderaa/local/software/Xilinx/Vivado/2020.1

ENV PATH=${PATH}:${OMPSS_ROOT}/mcxx-arm64/bin:${OMPSS_ROOT}/ait:${OMPSS_ROOT}/mcxx-x86_64/bin:${XILINX_VIVADO}/bin
ENV QEMU_LD_PREFIX=/usr/aarch64-linux-gnu

COPY entrypoint.sh /dockfpga/euroexa_entrypoint.sh
COPY dockfpga /dockfpga/

ENTRYPOINT ["/dockfpga/euroexa_entrypoint.sh"]
