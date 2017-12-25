# EXAMPLE

## The dataset
To train your neural network, you need at first to find a dataset. There
is a lot of examples of dataset on the UCI archive page [UCI](https://archive.ics.uci.edu)

For this example, I choose the Iris dataset, this is a CSV labelled dataset.
The first 4 columns are the input signal, the last column are labels.

## Network building
You can easily build a neural network to learn how to recognize Iris.
```
<?xml version="1.0"?>
<network inputs="4">
    <layers>
        <layer neurons="4"/>
        <layer neurons="7"/>
        <layer neurons="3"/>
    </layers>
</network>
```
## Network tweaking
Then you can tweaks its settings using XML:
```
<?xml version="1.0"?>
<settings cost-function="Quadratic" activation-function="Sigmoid">
    <training error="0.001" iterations="8000">
        <backprop learning-rate="1.2" momentum="0.001"/>
        <!--rprop eta-plus="1.25" eta-minus="0.55" delta-min="0.000001" delta-max="50.0"/-->
    </training>
</settings>
```
As you can see, you can choose between BackProp and RProp.
## Network training
I want to remove this step using an IHM, but you can try to write and compile the
following piece of code to train your neural network:
```
    BrainNetwork network = new_network_from_context(TEST_TRAIN_NETWORK_PATH);

    if (BRAIN_ALLOCATED(network))
    {
        configure_network_with_context(network, TEST_TRAIN_SETTINGS_PATH);
        train_network(network, TEST_DATA_PATH, ",", BRAIN_TRUE, BRAIN_FALSE);
        serialize_network(network, TEST_TRAIN_SERIALIZE_PATH);
    }
```
You can see that you can build, tweak, train and save your neural network
with only 5 lines of C codes.
## Result
By tweaking the network you can reduce the error rate:
![TrainingResults](http://pix.toile-libre.org/?img=1514225785.png)
