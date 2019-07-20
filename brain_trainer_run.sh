#!/bin/bash

PLUGIN_NAME=MLP
XML_FOLDER=./multi-layer-perceptron/example
TRAINER=./install/debug/libbrain-1.0.0/bin/BrainTrainer

export LD_LIBRARY_PATH=./install/debug/libbrain-1.0.0/lib/
$TRAINER -p MLP \
         -n $XML_FOLDER/test_train_network.xml \
         -s $XML_FOLDER/test_train_settings.xml \
         -d $XML_FOLDER/test_train_data.xml \
         -r /tmp/serialized_network.xml
