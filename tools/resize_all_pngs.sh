#!/bin/bash

convert '*.png' -set filename:fn '%[basename]-big' -scale 1000 '%[filename:fn].png'

convert '*-big.png' -set filename:fn '%[basename]-cleaned' -resize 1024x1024 -depth 8 '%[filename:fn].png'
