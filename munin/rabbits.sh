#!/bin/sh

# Monitor temperatures

if [ "$1" = "config" ]; then
    echo 'graph_title Rabbit Temperatures'
    echo 'graph_args --base 1000 -l 0'
    echo 'graph_vlabel Temperature (C)'
    echo 'graph_scale no'
    echo 'graph_category House'
    echo 'cage.label Cage'
    echo 'bedroom.label Sleeping Area'
    exit 0
fi

curl -o - <url_to_arduino_here>
