A MULTI-LAYER PERCEPTRON NETWORK
------------------------------------

## Presentation
This neural network is a multi-layer perceptron. It has been designed to let the user user several activation functions or cost functions. Moreover several algorithm have been integrated. You can now build a balanced neural
network using only XML files. It has been designed to a plugin in a parent application.

## How it works ?
Actually it works using 2 mains steps. The first one is an offline step in which the user provide training data to make the network learn. The second one is the online mode when the user feeds the network with unknown data.

## Algorithm
Several algorithms have been integrated to improve the training process:

### Back propagation
It is an algorithm that compute the gradient of the error according to each weight in order to correct each weight value. The error flows from the output layer to the input layer.
[More...](http://neuralnetworksanddeeplearning.com/chap2.html)

### Resilient propagation
It is an other way of updated the weight value but instead of using directly the gradient value. We only use the gradient sign to compute each weight corrections.
[More...](http://aass.oru.se/~lilien/ml/seminars/2007_03_12c-Markus_Ingvarsson-RPROP.pdf)

### Dropout
It is an basic algorithm to avoid overfitting.
[More...](https://www.cs.toronto.edu/~hinton/absps/JMLRdropout.pdf)

### Mini-batch
It is a algorithm to train the network. It is a common algorithm use to increase training speed and to avoid oscillations.
[More](http://ruder.io/optimizing-gradient-descent/)

## How to use it ?

### Defining the network architecture

The architecture of the network is defined via XMl. 
```
<?xml version="1.0"?>
<network inputs="5">
    <layers>
        <layer neurons="3"/>
        <layer neurons="6"/>
        <layer neurons="2"/>
    </layers>
</network>
```

You only need to specify the number of inputs. Then to select the number of neurons per layer.
Then you should use the following method to create a dully connected network:

```
BrainNetwork new_network_from_context(BrainString filepath)
```

The BrainNetwork is an opaque structure. You need to use the api to train and feed your network.

### Tuning your network

You can tune ypur network a'd choose several parameters. For example to define all settings for a backpropagated neural network.
You should write :

```
<?xml version="1.0"?>
<settings cost-function="Quadratic" activation-function="Sigmoid">
    <training error="0.00001" iterations="1000">
        <backprop learning-rate="0.5" momentum="0.0"/>
    </training>
</settings>
```
Now some explanation. 

* cost-function 
it is the method used to measure the dfference between the network'output and the desired output.
it is usefull during the training phase. There are two choice :
- Quadratic  That compute E =  (x -  y)^2
- CrossEntropy: that compute E = y *log(x) + (1- y)* log(x)

* Activation function
it is the method used to compute the output of the neuron.