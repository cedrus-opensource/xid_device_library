#!/bin/bash

export PATH="/Library/Frameworks/Python.framework/Versions/2.6/bin:$PATH"
export PYTHONPATH="$PYTHONPATH:../scons_helpers/scons/"
export PYTHONPATH="$PYTHONPATH:$SCONS_DIR"

/usr/local/bin/scons  "$@"