#include "brain_xml_utils.h"
#include "brain_logging_utils.h"

#include <libxml/xmlschemastypes.h>
#include <libxml/encoding.h>

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
    if (node)
    {
        Context child = node->xmlChildrenNode;
        BrainInt index = 0;

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
        xmlSchemaValidCtxtPtr validation_ctxt;

        validation_ctxt = xmlSchemaNewValidCtxt(schema);

        xmlSchemaSetValidErrors(validation_ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);

        const BrainInt ret = xmlSchemaValidateDoc(validation_ctxt, doc);
        if (ret == 0)
        {
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

        xmlSchemaFreeValidCtxt(validation_ctxt);
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

Writer
create_document(BrainString filepath, BrainString encoding)
{
    Writer writer = xmlNewTextWriterFilename(filepath, 0);

    if (writer != NULL)
    {
        if (0 <= xmlTextWriterStartDocument(writer, NULL, encoding, NULL))
        {
            xmlTextWriterSetIndent(writer, BRAIN_TRUE);

            return writer;
        }
        else
        {
            BRAIN_CRITICAL("%s", "Unable to open document for writing");
        }
    }
    else
    {
        BRAIN_CRITICAL("%s", "Unable to create document");
    }

    return NULL;
}

BrainBool
start_element(Writer writer,
                  BrainString element)
{
    if ((writer != NULL)
    &&  (element != NULL))
    {
        if (0 <= xmlTextWriterStartElement(writer, BAD_CAST element))
        {
            return BRAIN_TRUE;
        }
        else
        {
            BRAIN_CRITICAL("%s:%s", "Unable to create element", element);
        }
    }

    BRAIN_CRITICAL("Unable to create element: %s\n", element);

    return BRAIN_FALSE;
}

void
stop_element(Writer writer)
{
    if (writer != NULL)
    {
        xmlTextWriterEndElement(writer);
    }
}

void
add_attribute(Writer writer, BrainString attribute, BrainString value)
{
    if (writer != NULL && attribute != NULL && value != NULL)
    {
        xmlTextWriterWriteAttribute(writer, BAD_CAST attribute, BAD_CAST value);
    }
}

void
write_element(Writer writer, BrainString element, BrainString value)
{
    if (writer != NULL && element != NULL && value != NULL)
    {
        xmlTextWriterWriteElement(writer, BAD_CAST element, BAD_CAST value);
    }
}

void
close_writer(Writer writer)
{
    if (writer != NULL)
    {
        xmlTextWriterEndDocument(writer);
    }
}
