#!/bin/sh

standardese_bin="$1"
$standardese_bin --config standardese.ini --output.prefix=html/ ../include/crossguid/guid.hpp
