#!/bin/bash

_log() {
  if [ "$_DEBUG" == "true" ]; then
    echo 1>&2 "$@"
  fi
}


_log "Copying files..."
echo "Hello $USER,"
echo "Today is $(date +'%Y-%m-%d')"
_log "Done with Copying..."
