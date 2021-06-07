FROM ompss_at_fpga:2.5.2-euroexa

USER root

RUN addgroup --gid 1112 --system vanderaa && adduser --uid 1091 --system --ingroup vanderaa vanderaa

USER vanderaa
WORKDIR /home/vanderaa
