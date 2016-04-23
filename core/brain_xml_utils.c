#include "brain_xml_utils.h"

int
is_node_with_name(Context node, const char* name)
{
    if (!xmlStrcmp(node->name, (const xmlChar *)name))
    {
        return 1;
    }

    return 0;
}

Context
get_node_with_name_and_index(Context node, const char* name, const int i)
{
    int index = 0;
    Context child;

    if (node)
    {
        child = node->xmlChildrenNode;

        while (child)
        {
            if(is_node_with_name(child, name))
            {
                if (i == index)
                {
                    return child;
                }

                ++index;
            }

            child = child->next;
        }

    }

    return NULL;
}

int
get_number_of_node_with_name(Context node, const char* node_name)
{
    int number_of_node = 0;
    Context child;

    if (node)
    {
        child = node->xmlChildrenNode;

        while (child)
        {
            if(is_node_with_name(child, node_name))
            {
                ++number_of_node;
            }

            child = child->next;
        }
    }

    return number_of_node;
}

xmlChar*
node_get_prop(Context node, const char* key)
{
    xmlChar* res = NULL;

    if (node)
    {
        res = xmlGetProp(node, (const xmlChar*)key);
    }

    return res;
}

double
node_get_double(Context node, const char* key, const double _default)
{
    double value = _default;
    xmlChar* res = node_get_prop(node, key);

    if (res)
    {
        sscanf((char *)res, "%lf", &value);

        xmlFree(res);
    }

    return value;
}


int
node_get_int(Context node, const char* key, const int _default)
{
    int value = _default;
    xmlChar* res = node_get_prop(node, key);

    if (res)
    {
        sscanf((char *)res, "%d", &value);

        xmlFree(res);
    }

    return value;
}

Document
open_document(const char *doc)
{
    if (doc)
    {
        return xmlParseFile(doc);
    }

    return NULL;
}

void
close_document(Document doc)
{
    if (doc)
    {
        xmlFreeDoc(doc);
    }
}

Context
get_root_node(Document doc)
{
    if (doc)
    {
        return xmlDocGetRootElement(doc);
    }

    return NULL;
}
