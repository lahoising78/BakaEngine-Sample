#! /bin/bash

for vert in shaders/*.vert
do
    glslc "${vert}" -o "${vert%.vert}_vert.sprv"
done

for frag in shaders/*.frag
do
    glslc "${frag}" -o "${frag%.frag}_frag.sprv"
done