#!/bin/bash

VK_PATH=/opt/vulkan-sdk/1.1.101.0/x86_64/

for file in $(ls *.vert )
do
  $VK_PATH/bin/glslangValidator -V -o $file.spv $file
done

for file in $(ls *.frag )
do
  $VK_PATH/bin/glslangValidator -V -o $file.spv $file
done
