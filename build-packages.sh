IMAGE_NAME="builder-18.04"

docker run --rm -v $(pwd):/builder -w /builder $IMAGE_NAME \
    bash -c "cmake -B build -S . && \
             cmake --build build -j$(nproc) && \
             cd build && \
             cpack -G DEB && \
             cpack -G RPM && \
             chown -R $(id -u):$(id -g) /builder/build"

ls -lh build/*.deb build/*.rpm
