#!/bin/bash

HOME=~/.electron-gyp node-gyp configure rebuild --target=1.7.5 --arch=x64 --dist-url=https://atom.io/download/electron --debug