# A MULTI-LAYER PERCEPTRON NETWORK
------------------------------------

## Presentation
This neural network is a multi-layer perceptron. It has been designed to
let the user user several activation functions or cost functions. Moreover
several algorithm have been integrated. You can now build a balanced neural
network using only XML files. It has been designed to a plugin in a parent
application.

## How it works ?
Actually it works using 2 mains steps. The first one is an offline step in
which the user provide training data to make the network learn. The second
one is the online mode when the user feeds the network with unknown data.

## Algorithm
Several algorithms have been integrated to improve the training process:

### Back propagation
It is an algorithm that compute the gradient of the error according to each
weight in order to correct each weight value. The error flows from the output
layer to the input layer.
[More...](http://neuralnetworksanddeeplearning.com/chap2.html)

### Resilient propagation
It is an other way of updated the weight value but instead of using directly
the gradient value. We only use the gradient sign to compute each weight
corrections.
[More...](http://aass.oru.se/~lilien/ml/seminars/2007_03_12c-Markus_Ingvarsson-RPROP.pdf)

### Dropout
It is an basic algorithm to avoid overfitting.
[More...](https://www.cs.toronto.edu/~hinton/absps/JMLRdropout.pdf)

### Mini-batch
It is a algorithm to train the network. It is a common algorithm use to
increase training speed and to avoid oscillations.
[More](http://ruder.io/optimizing-gradient-descent/)
