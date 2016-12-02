#!/bin/bash
protoc  --plugin=protoc-gen-nanopb=nanopb/generator/protoc-gen-nanopb  --nanopb_out=. AirSensor.proto
