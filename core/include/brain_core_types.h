/**
 * \file brain_core_types.h
 * \brief Define all types
 * \author Benoit F.
 * \date 11 decembre 2016
 *
 * This file contains all predefines types used in this library
 */
#ifndef BRAIN_CORE_TYPES_H
#define BRAIN_CORE_TYPES_H

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xmlwriter.h>

/**
 * \def BRAIN_TRUE
 */
#define BRAIN_TRUE  1
/**
 * \def BRAIN_FALSE
 */
#define BRAIN_FALSE 0

typedef unsigned char BrainBool;
typedef int           BrainInt;
typedef unsigned int  BrainUint;
typedef float         BrainFloat;
typedef double        BrainDouble;
typedef const char*   BrainString;
typedef char          BrainChar;
#if BRAIN_ENABLE_DOUBLE_PRECISION
typedef BrainDouble BrainReal;
#else
typedef BrainFloat BrainReal;
#endif
/**
 * \brief Define a Data parser
 */
typedef enum DataParser
{
    Parser_CSV,
    Parser_Invalide,
    Parser_First = Parser_CSV,
    Parser_Last  = Parser_Invalide
} DataParser;
/**
 * \brief Define a Data preprocessing
 */
typedef enum DataPreprocessing
{
    Preprocessing_GaussianNormalization,
    Preprocessing_MinMaxNormalization,
    Preprocessing_Invalide,
    Preprocessing_First = Preprocessing_GaussianNormalization,
    Preprocessing_Last  = Preprocessing_Invalide
} DataPreprocessing;
/**
 * \brief define a DataFormat
 */
typedef enum BrainDataFormat
{
    Format_InputFirst,
    Format_OutputFirst,
    Format_Invalide,
    Format_First = Format_InputFirst,
    Format_Last  = Format_Invalide
} BrainDataFormat;
/**
 * \brief Define how to build a BrainData
 */
typedef struct DataParameters
{
    BrainString         repository_path;
    BrainString         tokenizer;
    BrainUint           input_length;
    BrainUint           output_length;
    DataParser          parser;
    BrainBool           is_labedelled;
    BrainDataFormat     format;
    BrainUint           number_of_preprocessing;
    DataPreprocessing*  preprocessings;
} DataParameters;
/**
 * \brief Define a pointer to a Data Parameters struct
 */
typedef DataParameters* BrainDataParameters;
/**
 * \brief define a XML Context
 */
typedef xmlNodePtr Context;
/**
 * \brief define an XML Document
 */
typedef xmlDocPtr Document;
/**
 * \brief Define an XML buffer
 */
typedef xmlChar*  Buffer;
/**
 * \brief define an XML writer
 */
typedef xmlTextWriterPtr Writer;
/**
 * \brief Define a BrainRandomMask
 */
typedef struct RandomMask* BrainRandomMask;
/**
 * \brief Define a CsvReader
 */
typedef struct CsvReader* BrainCsvReader;
/**
* \brief Define a BrainSignal using single or double precision
*/
typedef BrainReal* BrainSignal;
/**
 * \brief Define a BrainNetwork
 */
typedef struct Network* BrainNetwork;
/**
 * \brief Define a BrainData
 */
typedef struct Data* BrainData;
#endif /* BRAIN_CORE_TYPES_H */
