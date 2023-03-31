ARG REGISTRY_PREFIX='wagoautomation/'
ARG SDK_BUILDER_VERSION='1.0.0'

FROM ${REGISTRY_PREFIX}sdk-builder:${SDK_BUILDER_VERSION} as pfc-firmware-sdk-G2


ARG USERID
RUN useradd -u "$USERID" -ms /bin/bash user
RUN chown -R user:user /home/user 
COPY --chown=user:user build.sh /usr/local/bin/build
COPY --chown=user:user certs/* /usr/local/share/ca-certificates/
RUN update-ca-certificates
