/** \internal
 * \file brain_xml_utils.h
 * \brief Define the API to load an XML file
 * \author Benoit F.
 * \date 16 decembre 2016
 *
 * All public methods to read an XML file
 */
#ifndef BRAIN_XML_UTILS_H
#define BRAIN_XML_UTILS_H

#include "brain_types.h"

/**
 * \fn BrainBool is_node_with_name(Context node, BrainString name)
 * \brief Check if a give node has a certain name
 *
 * \param node An XML Node
 * \param name Potential node name
 * \return a BrainBool if the node has the good name
 */
BrainBool is_node_with_name(Context node,
                            BrainString name);
/**
 * \fn Context get_node_with_name_and_index(Context node,
 *                                          BrainString name,
 *                                          const BrainInt i)
 * \brief get a child node with its name and its position in the XML
 *
 * \param node The current node
 * \param name The potential child node name
 * \param i    Index of the child node
 * \return a child node
 */
Context get_node_with_name_and_index(Context node,
                                     BrainString name,
                                     const BrainInt i);
/**
 * \fn BrainInt get_number_of_node_with_name(Context node,
 *                                           BrainString node_name)
 * \brief get the number of children node with given node_name name
 *
 * \param node Current XML node
 * \param node_name Child node potential name
 * \return the number of children node with name node_name
 */
BrainInt get_number_of_node_with_name(Context node,
                                      BrainString node_name);
/**
 * \fn Buffer node_get_prop(Context node, BrainString key)
 * \brief get a string buffer from an XML node
 *
 * \param node The current node
 * \param key  The attribute name
 * \return an XML buffer that shoul be free
 */
Buffer node_get_prop(Context node,
                     BrainString key);
/**
 * \fn BrainInt node_get_int(Context node, BrainString key, const BrainInt _default)
 * \brief extract an int value from an attribute
 *
 * \param node The current node
 * \param key  The attribute name
 * \param _default The default value
 * \return a BrainInt
 */
BrainInt node_get_int(Context node,
                      BrainString key,
                      const BrainInt _default);
/**
 * \fn BrainDouble node_get_double(Context node, BrainString key, const BrainDouble _default)
 * \brief extract a double value from an attribute
 *
 * \param node The current node
 * \param key  The attribute name
 * \param _default The default value
 * \return a BrainDouble
 */
BrainDouble node_get_double(Context node,
                            BrainString key,
                            const BrainDouble _default);

/**
 * \fn BrainBool node_get_bool(Context node,
 *                             BrainString key,
 *                             const BrainBool _default)
 * \brief retrieve a boolean from a node
 *
 * \param node the current node
 * \param key  the sttribute name
 * \param _default default value
 * \return a boolean
 */
BrainBool   node_get_bool(Context node,
                          BrainString key,
                          const BrainBool _default);
/**
 * \fn Document open_document(BrainString doc)
 * \brief open an XML document
 *
 * \param doc filepath of the requested XML file
 * \return an opened XML document
 */
Document open_document(BrainString doc);
/**
 * \fn void close_document(Document doc)
 * \brief close an XML document
 *
 * \param doc filepath of the closed XML file
 */
void     close_document(Document doc);
/**
 * \fn Context get_root_node(Document doc)
 * \brief get the root node of an XML documentt
 *
 * \param doc An XML document
 * \return a XML node
 */
Context  get_root_node(Document doc);
/**
 * \fn BrainBool validate_with_xsd(BrainString xml_file, BrainString xsd_file)
 * \brief Validate an XML file against an XSD schema
 *
 * \param xml_file An XML file
 * \param xsd_file An XSD file
 * \return whether or not this XML file is valid for the given XSD file
 */
BrainBool validate_with_xsd(BrainString xml_file,
                            BrainString xsd_file);

/**
 * \fn Writer create_document(BrainString filepath, BrainString encoding)
 * \brief create a new document
 *
 * \param filepath
 * \param encoding
 * \return the XML writer
 */
Writer create_document(BrainString filepath, BrainString encoding);
/**
 * \fn BrainBool start_element(Writer writer, BrainString element)
 * \brief start a new element
 *
 * \param writer the XML writer
 * \param element new element name
 *
 * \param return true if the element has been added
 */
BrainBool start_element(Writer writer, BrainString element);
/**
 * \fn void stop_element(Writer writer)
 * \brief stop an element
 *
 * \param writer the XML writer
 */
void stop_element(Writer writer);
/**
 * \fn void add_attribute(Writer writer, BrainString attribute, BrainString value)
 * \brief add a new attribute
 *
 * \param writer the XML writer
 * \param attribute the new attribute
 * \param value the attribute value
 */
void add_attribute(Writer writer, BrainString attribute, BrainString value);
/**
 * \fn void write_element(Writer writer, BrainString element, BrainString value)
 * \brief add a new attribute
 *
 * \param writer the XML writer
 * \param element the new element
 * \param value the element value
 */
void write_element(Writer writer, BrainString element, BrainString value);
/**
 * \fn void close_writer(Writer writer)
 * \brief close the XML writer
 *
 * \param writer the writter to be close
 */
void close_writer(Writer writer);
/**
 * \fn BrainDouble node_get_content_as_double(Context node)
 * \brief grab the content of a node as a double value
 *
 * \param a node
 * \return the content as a double value
 */
BrainDouble node_get_content_as_double(Context node);
#endif /*BRAIN_XML_UTILS_H*/
