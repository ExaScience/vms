FROM dockcross/linux-arm64
MAINTAINER Tom Vander Aa "tom.vanderaa@imec.be"

ENV DEFAULT_DOCKCROSS_IMAGE euroexacross

ADD ompss_at_fpga.tar.gz /

ENV OMPSS_ROOT=/opt/bsc/x86_64/ompss/2.5.2-euroexa

ENV PATH=${PATH}:${OMPSS_ROOT}/mcxx-arm64/bin:${OMPSS_ROOT}/ait:${OMPSS_ROOT}/mcxx-x86_64/bin

COPY entrypoint.sh /dockcross/euroexa_entrypoint.sh
COPY dockcross /dockcross/

ENTRYPOINT ["/dockcross/euroexa_entrypoint.sh"]
