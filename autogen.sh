#!/bin/sh

set -e

touch README
autoreconf -vi
rm README
