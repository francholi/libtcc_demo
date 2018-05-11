#!/bin/bash
sleep 1.0
sed -e 's/glClearColor(1/glClearColor(0/g' level0.c > level0.c.new && mv level0.c.new level0.c
sleep 1.0
sed -e 's/glClearColor(0/glClearColor(1/g' level0.c > level0.c.new && mv level0.c.new level0.c

cat changeFile.sh 1>&2
