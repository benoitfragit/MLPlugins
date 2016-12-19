#include "brain_xml_utils.h"
#include "brain_logging_utils.h"

#include <libxml/xmlschemastypes.h>

BrainBool
is_node_with_name(Context node, BrainString name)
{
    if (!xmlStrcmp(node->name, (const Buffer)name))
    {
        return BRAIN_TRUE;
    }

    return BRAIN_FALSE;
}

Context
get_node_with_name_and_index(Context node,
                             BrainString name,
                             const BrainInt i)
{
    BrainInt index = 0;
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

BrainInt
get_number_of_node_with_name(Context node,
                             BrainString node_name)
{
    BrainInt number_of_node = 0;
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

Buffer
node_get_prop(Context node, BrainString key)
{
    xmlChar* res = NULL;

    if (node)
    {
        res = xmlGetProp(node, (const xmlChar*)key);
    }

    return res;
}

BrainBool
node_get_bool(Context node,
              BrainString key,
              const BrainBool _default)
{
    BrainBool value = _default;
    Buffer res = node_get_prop(node, key);

    if (res)
    {
        if (!strcmp((BrainString)res, "true"))
        {
            value = BRAIN_TRUE;
        }
        else if (atoi((BrainString)res) == 1)
        {
            value = BRAIN_TRUE;
        }

        xmlFree(res);
    }

    return value;
}

BrainDouble
node_get_double(Context node,
                BrainString key,
                const BrainDouble _default)
{
    BrainDouble value = _default;
    Buffer res = node_get_prop(node, key);

    if (res)
    {
        value = atof((BrainString)res);

        xmlFree(res);
    }

    return value;
}

BrainInt
node_get_int(Context node,
             BrainString key,
             const BrainInt _default)
{
    BrainInt value = _default;
    Buffer res = node_get_prop(node, key);

    if (res)
    {
        value = atoi((BrainString)res);

        xmlFree(res);
    }

    return value;
}

Document
open_document(BrainString doc)
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

BrainBool
validate_with_xsd(BrainString xml_file, BrainString xsd_file)
{
    BrainInt ret = 0;
    BrainBool result = BRAIN_FALSE;
    Document doc;
    xmlSchemaPtr schema = NULL;
    xmlSchemaParserCtxtPtr ctxt;

    xmlLineNumbersDefault(1);

    ctxt = xmlSchemaNewParserCtxt(xsd_file);

    xmlSchemaSetParserErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
    schema = xmlSchemaParse(ctxt);
    xmlSchemaFreeParserCtxt(ctxt);

    doc = xmlReadFile(xml_file, NULL, 0);

    if (doc == NULL)
    {
        BRAIN_CRITICAL("Unable to open %s", xml_file);
        return result;
    }
    else
    {
        xmlSchemaValidCtxtPtr ctxt;

        ctxt = xmlSchemaNewValidCtxt(schema);
        xmlSchemaSetValidErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
        ret = xmlSchemaValidateDoc(ctxt, doc);
        if (ret == 0)
        {
            BRAIN_DEBUG("%s validates\n", xml_file);
            result = BRAIN_TRUE;
        }
        else if (ret > 0)
        {
            BRAIN_DEBUG("%s fails to validate\n", xml_file);
        }
        else
        {
            BRAIN_CRITICAL("%s validation generated an BrainInternal error\n", xml_file);
        }

        xmlSchemaFreeValidCtxt(ctxt);
        xmlFreeDoc(doc);
    }

    if(schema != NULL)
    {
        xmlSchemaFree(schema);
    }

    xmlSchemaCleanupTypes();
    xmlCleanupParser();
    xmlMemoryDump();

    return result;
}
