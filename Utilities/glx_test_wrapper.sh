#!/bin/bash

xvfb-run --auto-servernum --server-num=1 --server-args="-screen 0 1024x768x24 +extension GLX +render -noreset" $1

