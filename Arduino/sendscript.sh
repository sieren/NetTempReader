#!/bin/bash

def_host=192.168.8.136
def_port=4210

HOST=${2:-$def_host}
PORT=${3:-$def_port}

echo -n "$1" | nc -4u -w1 $HOST $PORT
