#!/bin/bash

set -e
export LANG=C

if [ ! -e testlist ]; then
    echo "testlist not found, please use make"
    exit 255
fi

echo ""
for t in `cat testlist`; do
    t=$(basename $t)
    cd $t
    echo -n "Checking $t...: "

    if diff -q output.ok output; then
        echo "OK"
        cd ..
        continue
    fi

    diff -u output.ok output || true
    echo ""
    echo "*** $t FAILED [$(cat README)]"
    echo ""
    cd ..
    exit 255
done

echo ""
echo "....... All's well, ends well ....... ;-)"
echo ""
