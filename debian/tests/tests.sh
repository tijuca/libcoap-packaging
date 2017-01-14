#!/bin/sh

set -e
#set -x

cd debian/tests

echo -n "Compiling, linking the CoAP testing server using C source ... "
gcc -g -o coap-test-server coap-test-server.c -O2 -D_GNU_SOURCE -DWITH_POSIX `pkg-config --libs libcoap-1`
if [ "$?" != "0" ]; then
    echo "Failed!"
    exit 1
else
    echo "Successful!"
fi

echo -n "Starting the CoAP testing server ... "
./coap-test-server &

PID_COAP_SERVER=$(pidof coap-test-server)
if [ "${PID_COAP_SERVER}" = "" ]; then
    echo "Failed!"
    exit 1
else
    echo "Successful!"
fi

echo -n  "Probing CoAP testing server using IPv4 ... "
RET=$(coap-client -m get coap://127.0.0.1/ | grep CoAP)

if [ "${RET}" = "" ]; then
    echo "Failed!"
    exit 1
else
    echo "Successful!"
fi

RET=""

echo -n "Probing CoAP testing server using IPv6 ... "
RET=$(coap-client -m get coap://[::1]/ | grep CoAP)

if [ "${RET}" = "" ]; then
    echo "Failed!"
    exit 1
else
    echo "Successful!"
fi

RET=""

kill ${PID_COAP_SERVER}

echo "[HINT] Compiling, linking the CoAP testing server using C++ source is still needed to add here."

echo 'Testing coap-test-server was successful!'
exit 0
