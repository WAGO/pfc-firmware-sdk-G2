IMAGE_NAME=pfc-firmware-base
docker buildx build --platform=linux/arm/v7 -t $IMAGE_NAME --load -f Dockerfile ..