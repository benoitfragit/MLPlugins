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
* Learning type (RProp, BackProp) with several parameters (max iteration, target error)
* Activation function (Sigmoid, SoftPlus, TanH, ...)
* Cost function (Quadratic, CrossEntropy)

Tutorial
========

How to define a BackProp Network
--------------------------------
```xml
<?xml version="1.0"?>
<network inputs="2">
    <layers>
        <layer neurons="2"/>
        <layer neurons="3"/>
        <layer neurons="2"/>
    </layers>

    <settings activation-function="Sigmoid" cost-function="CrossEntropy">
        <training learning="BackPropagation" iterations="10000" error="0.01">
            <dropout activate="false" factor="0.5"/>
            <method>
                <backprop learning-rate="1.2"/>
            </method>
        </training>
    </settings>
</network>
```

How to define the same network with RProp
-----------------------------------------
```xml
<?xml version="1.0"?>
<network inputs="2">
    <layers>
        <layer neurons="2"/>
        <layer neurons="3"/>
        <layer neurons="2"/>
    </layers>

    <settings activation-function="Sigmoid" cost-function="CrossEntropy">
        <training learning="Resilient" iterations="10000" error="0.01">
            <dropout activate="false" factor="0.5"/>
            <method>
                <rprop>
                    <eta   positive="1.2" negative="0.95"/>
                    <delta min="0.000001" max="50.0"/>
                </rprop>
            </method>
        </training>
    </settings>
</network>
```

Working with Neural Network
============================

How can I tune my network ?
---------------------------

- Change the Neuron Activation function using "activation-function" attribute
- Change the Network Cost function using "cost-function" attribute
- Change the Neuron training method using learning attribute
- Change the Neuron training duration and accuracy using "iterations" and "error" attributes
- In BackProp mode, change the learning rate
- In RProp mode, change several parameters (eta, delta) used during the training process
- Activate and tune dropout with "activate" and "factor" attributes


I Have a functional network how to save it
-------------------------------------------

You can save the state of a network to reload it later, here is an example of how the network weights/bias will be saved:

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
Once you have an XML definition file for you network, you can load it:

```C
BrainNetwork network = new_network_from_context(BrainString filepath);
```

Then you can load Training dataset using

```C
BrainData data = new_data_from_context(BrainString filepath);
```

Then you can start the training process using:

```C
BrainBool train(BrainNetwork network, const BrainData data);
```

Finally in online mode you can simple use the feedforward method of the network to feed the network
and grab the output of the layer like this:

```C
void feedforward(BrainNetwork network, const BrainUint number_of_input, const BrainSignal in);
BrainSignal get_network_output(const BrainNetwork network);
```

You can access the full libBrain documentation here:

http://benoitfragit.github.io/libBrain/
