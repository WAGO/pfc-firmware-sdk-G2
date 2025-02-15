ARG REGISTRY_PREFIX='wagoautomation/'
ARG SDK_BUILDER_VERSION='3.0.1'

FROM ${REGISTRY_PREFIX}sdk-builder:${SDK_BUILDER_VERSION} AS pfc-firmware-sdk-G2


ARG USERID
RUN useradd -l -u "$USERID" -ms /bin/bash user
RUN chown -R user:user /home/user 
COPY --chown=user:user build.sh /usr/local/bin/build
COPY --chown=user:user certs/* /usr/local/share/ca-certificates/
RUN update-ca-certificates
