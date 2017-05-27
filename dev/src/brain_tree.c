#include "brain_tree.h"
#include "brain_random.h"

/**
 * \struct Node
 * \brief  Internal model for a BrainNode
 *
 * All protected fields for a BrainNode
 */
struct Node
{
    BrainSignal _input;  /*!< The input signal */
    BrainSignal _output; /*!< The output signal */
    BrainNode   _left;   /*!< The left node */
    BrainNode   _right;  /*!< The right node */
    BrainUint   _children; /*!< number of children */
} Node;

/**
 * \struct Tree
 * \brief  Internal model for a BrainTree
 *
 * All protected fields for a BrainTree
 */
struct Tree
{
    BrainNode _training;         /*!< This is the training node */
    BrainNode _evaluating;       /*!< This is the evaluating node */
    BrainBool _is_data_splitted; /*!< is data automatically splitter */
    BrainUint _input_length;     /*!< input signal length */
    BrainUint _output_length;    /*!< output signal length */
} Tree;

BrainTree
new_tree(const BrainUint input_length,
         const BrainUint output_length,
         const BrainBool is_data_splitted)
{
    BrainTree _tree = (BrainTree)calloc(1, sizeof(Tree));

    _tree->_training   = NULL;
    _tree->_evaluating = NULL;
    _tree->_is_data_splitted = is_data_splitted;
    _tree->_input_length = input_length;
    _tree->_output_length = output_length;

    return _tree;
}

static void
delete_node(BrainNode node)
{
    if (node != NULL)
    {
        delete_node(node->_left);

        delete_node(node->_right);

        if (node->_input != NULL)
        {
            free(node->_input);
        }

        if (node->_output != NULL)
        {
            free(node->_output);
        }

        free(node);

        node = NULL;
    }
}

static void
create_child_node(BrainNode node, BrainSignal input, BrainSignal output)
{
    if (node != NULL)
    {
        if (node->_left == NULL)
        {
            node->_children += 1;
            create_child_node(node->_left, input, output);
        }
        else if (node->_right == NULL)
        {
            node->_children += 1;
            create_child_node(node->_right, input, output);
        }
    }
    else
    {
        node = (BrainNode)calloc(1, sizeof(Node));

        node->_input  = input;
        node->_output = output;
        node->_left   = NULL;
        node->_right  = NULL;

        node->_children = 1;
    }
}

void
delete_tree(BrainTree tree)
{
    if (tree != NULL)
    {
        delete_node(tree->_training);
        delete_node(tree->_evaluating);

        free(tree);
    }
}

BrainNode
get_training_node(const BrainTree tree)
{
    if (tree != NULL)
    {
        return tree->_training;
    }

    return NULL;
}

BrainNode
get_evaluating_node(const BrainTree tree)
{
    if (tree != NULL)
    {
        return tree->_evaluating;
    }

    return NULL;
}

BrainNode
get_left_node(const BrainNode node)
{
    if (node != NULL)
    {
        return node->_left;
    }

    return NULL;
}

BrainNode
get_right_node(const BrainNode node)
{
    if (node != NULL)
    {
        return node->_right;
    }

    return NULL;
}

BrainSignal
get_node_input_signal(const BrainNode node)
{
    if (node != NULL)
    {
        return node->_input;
    }

    return NULL;
}

BrainSignal
get_node_output_signal(const BrainNode node)
{
    if (node != NULL)
    {
        return node->_output;
    }

    return NULL;
}

void
new_node(BrainTree tree, BrainSignal input, BrainSignal output)
{
    if (tree != NULL)
    {
        // data auto separation into 2 sets testing
        if ((tree->_is_data_splitted == BRAIN_TRUE) && get_random_double_value() < 0.5)
        {
            create_child_node(tree->_training, input, output);
        }
        else
        {
            create_child_node(tree->_evaluating, input, output);
        }
    }
}

BrainUint
get_input_signal_length(const BrainTree tree)
{
    if (tree != NULL)
    {
        return tree->_input_length;
    }

    return 0;
}

BrainUint
get_output_signal_length(const BrainTree tree)
{
    if (tree != NULL)
    {
        return tree->_output_length;
    }

    return 0;
}

BrainUint
get_node_children(const BrainNode node)
{
    if (node != NULL)
    {
        return node->_children;
    }

    return 0;
}
