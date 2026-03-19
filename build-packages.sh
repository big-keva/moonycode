IMAGE_NAME="builder-18.04"

docker run --rm -v $(pwd):/builder -w /builder $IMAGE_NAME \
    bash -c "cmake -B build -S . \
             -DCMAKE_INSTALL_PREFIX=/usr \
             -DCMAKE_INSTALL_LIBDIR=lib \
             -DCMAKE_INSTALL_INCLUDEDIR=include && \
             cmake --build build -j$(nproc) && \
             cd build && \
             cpack -G DEB && \
             cpack -G RPM && \
             chown -R $(id -u):$(id -g) /builder/build"

ls -lh build/*.deb build/*.rpm
