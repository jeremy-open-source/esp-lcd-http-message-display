#!/usr/bin/env bash

HOST=$1
MESSAGE=${2:-"Test message here"}
AUTH_CODE=${3:-1234567890}
DURATION=${4:-10000}

curl \
    -X POST \
    -H "Auth-Token: ${AUTH_CODE}" \
    --data-urlencode "message=${MESSAGE}" \
    --data-urlencode "duration=${DURATION}" \
    http://${HOST}/display
