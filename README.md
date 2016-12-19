Presentation
=================
The goal of the libBrain project is to develop a very flexible way to build, train an use a neural network.
it offers:

Reading and Writing
-------------------
* XML network structure loading
* XML network initialization
* XML validation with XSD schema
* XML network serialization

Training
--------
Several parameters could be changed very easily from the network XML definition:

* Dropout parameters to exclude some neuron from training to avoid overfitting
* Learning rate
* Activation function (Sigmoid, SoftPlus, TanH, ...)
* Cost function (Quadratic, CrossEntropy)

Tutorial
========

How to write an network
-----------------------
```xml
<?xml version="1.0"?>
<network inputs="2" use-dropout="false" dropout-percent="0.5" learning-rate="1.2" activation-function-type="Sigmoid" cost-function-type="CrossEntropy">
    <layer neurons="2"/>
    <layer neurons="3"/>
    <layer neurons="2"/>
</network>
```

You can save the state of a network to reload it later, here is an example:

```xml
<init>
    <neuron index="0" layer-index="0" bias="0.164150">
        <weight value="0.229889" input="0"/>
        <weight value="0.005664" input="1"/>
    </neuron>
    <neuron index="1" layer-index="0" bias="0.325530">
        <weight value="-0.369910" input="0"/>
        <weight value="-0.379591" input="1"/>
    </neuron>
    <neuron index="0" layer-index="1" bias="0.465258">
        <weight value="0.137685" input="0"/>
        <weight value="0.199213" input="1"/>
    </neuron>
    <neuron index="1" layer-index="1" bias="-0.208207">
        <weight value="-0.177604" input="0"/>
        <weight value="0.183424" input="1"/>
    </neuron>
    <neuron index="2" layer-index="1" bias="0.247561">
        <weight value="0.226107" input="0"/>
        <weight value="0.103306" input="1"/>
    </neuron>
    <neuron index="0" layer-index="2" bias="-0.144994">
        <weight value="0.017281" input="0"/>
        <weight value="-0.011214" input="1"/>
        <weight value="-0.114676" input="2"/>
    </neuron>
    <neuron index="1" layer-index="2" bias="-0.326887">
        <weight value="-0.156190" input="0"/>
        <weight value="-0.052734" input="1"/>
        <weight value="0.210257" input="2"/>
    </neuron>
</init>
```

API Documentation
=================
You can access libBrain documentation here:

http://benoitfragit.github.io/libBrain/
