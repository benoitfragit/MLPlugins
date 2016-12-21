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

How to write a Network
-----------------------
```xml
<?xml version="1.0"?>
<network inputs="2">
    <!-- define the structure of the Network -->
    <layers>
        <layer neurons="2"/>
        <layer neurons="3"/>
        <layer neurons="2"/>
    </layers>

    <!-- Configure the Network -->
    <settings activation-function="Sigmoid" cost-function="CrossEntropy">
        <training learning="BackPropagation" backpropagation-learning-rate="1.2">
            <dropout activate="false" factor="0.5"/>
            <resilient-eta   positive="1.2" negative="0.95"/>
            <resilient-delta min="0.000001" max="50.0"/>
        </training>
    </settings>
</network>
```

You can save the state of a network to reload it later, here is an example:

```xml
<init>
    <neuron index="0" layer-index="0" bias="0.387371">
        <weight value="0.064384"/>
        <weight value="-0.030312"/>
    </neuron>
    <neuron index="1" layer-index="0" bias="-0.232962">
        <weight value="-0.480723"/>
        <weight value="-0.078180"/>
    </neuron>
    <neuron index="0" layer-index="1" bias="0.025801">
        <weight value="-0.399538"/>
        <weight value="0.346602"/>
    </neuron>
    <neuron index="1" layer-index="1" bias="-0.374252">
        <weight value="-0.119340"/>
        <weight value="0.052492"/>
    </neuron>
    <neuron index="2" layer-index="1" bias="-0.365058">
        <weight value="-0.042249"/>
        <weight value="0.139676"/>
    </neuron>
    <neuron index="0" layer-index="2" bias="0.164984">
        <weight value="0.092912"/>
        <weight value="-0.011035"/>
        <weight value="0.077357"/>
    </neuron>
    <neuron index="1" layer-index="2" bias="-0.155481">
        <weight value="-0.104649"/>
        <weight value="0.052773"/>
        <weight value="-0.332921"/>
    </neuron>
</init>
```

API Documentation
=================
You can access libBrain documentation here:

http://benoitfragit.github.io/libBrain/
