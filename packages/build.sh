PARENT_DIR=$(dirname $(pwd))

docker run --rm -v "$PARENT_DIR:"/moonycode -v $(pwd):/root/rpmbuild/RPMS -w /moonycode/packages builder-18.04 \
    bash -c "pwd && ls && make $1"
