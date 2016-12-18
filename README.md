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
<network inputs="2" use-dropout="0" dropout-percent="0.5" learning-rate="1.2" activation-function-type="Sigmoid" cost-function-type="CrossEntropy">
    <layer neurons="2"/>
    <layer neurons="3"/>
    <layer neurons="2"/>
</network>
```

API Documentation
=================
You can access libBrain documentation here:

http://benoitfragit.github.io/libBrain/
