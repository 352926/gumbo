/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2013 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_gumbo.h"

#include "gumbo.h"

/* If you declare any globals in php_gumbo.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(gumbo)
*/

/* True global resources - no need for thread safety here */
static int le_gumbo_output;
#define le_gumbo_output_name "Gumbo Output"
static int le_gumbo_node;
#define le_gumbo_node_name "Gumbo Node"

/* {{{ gumbo_functions[]
 *
 * Every user visible function must have an entry in gumbo_functions[].
 */
const zend_function_entry gumbo_functions[] = {
	PHP_FE(gumbo_parse,	NULL)
	PHP_FE(gumbo_destroy_output, NULL)
	PHP_FE(gumbo_output_get_root, NULL)
	PHP_FE(gumbo_node_get_type, NULL)
	PHP_FE(gumbo_element_get_tag_name, NULL)
	PHP_FE(gumbo_element_get_children, NULL)
	PHP_FE(gumbo_text_get_text, NULL)
	PHP_FE(gumbo_element_get_attributes, NULL)
	PHP_FE(gumbo_element_get_tag_open, NULL)
	PHP_FE(gumbo_element_get_tag_close, NULL)
	PHP_FE_END	/* Must be the last line in gumbo_functions[] */
};
/* }}} */

/* {{{ gumbo_module_entry
 */
zend_module_entry gumbo_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"gumbo",
	gumbo_functions,
	PHP_MINIT(gumbo),
	PHP_MSHUTDOWN(gumbo),
	PHP_RINIT(gumbo),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(gumbo),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(gumbo),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_GUMBO
ZEND_GET_MODULE(gumbo)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("gumbo.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_gumbo_globals, gumbo_globals)
    STD_PHP_INI_ENTRY("gumbo.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_gumbo_globals, gumbo_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_gumbo_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_gumbo_init_globals(zend_gumbo_globals *gumbo_globals)
{
	gumbo_globals->global_value = 0;
	gumbo_globals->global_string = NULL;
}
*/
/* }}} */

static void php_gumbo_free_output( zend_rsrc_list_entry *rsrc TSRMLS_DC );
static void php_gumbo_free_node( zend_rsrc_list_entry *rsrc TSRMLS_DC );

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(gumbo)
{
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/

	REGISTER_LONG_CONSTANT( "GUMBO_NODE_DOCUMENT", GUMBO_NODE_DOCUMENT, CONST_CS | CONST_PERSISTENT );
	REGISTER_LONG_CONSTANT( "GUMBO_NODE_ELEMENT", GUMBO_NODE_ELEMENT, CONST_CS | CONST_PERSISTENT );
	REGISTER_LONG_CONSTANT( "GUMBO_NODE_TEXT", GUMBO_NODE_TEXT, CONST_CS | CONST_PERSISTENT );
	REGISTER_LONG_CONSTANT( "GUMBO_NODE_CDATA", GUMBO_NODE_CDATA, CONST_CS | CONST_PERSISTENT );
	REGISTER_LONG_CONSTANT( "GUMBO_NODE_COMMENT", GUMBO_NODE_COMMENT, CONST_CS | CONST_PERSISTENT );
	REGISTER_LONG_CONSTANT( "GUMBO_NODE_WHITESPACE", GUMBO_NODE_WHITESPACE, CONST_CS | CONST_PERSISTENT );

	le_gumbo_output = zend_register_list_destructors_ex( php_gumbo_free_output, NULL, le_gumbo_output_name, module_number );
	le_gumbo_node = zend_register_list_destructors_ex( php_gumbo_free_node, NULL, le_gumbo_node_name, module_number );

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(gumbo)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(gumbo)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(gumbo)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(gumbo)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "gumbo support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

/* {{{ proto string gumbo_parse(string html) */
PHP_FUNCTION(gumbo_parse)
{

	char * html = NULL;
	int htmlLength;
	if ( zend_parse_parameters( ZEND_NUM_ARGS() TSRMLS_CC, "s", &html, &htmlLength ) == FAILURE ) {
		return;
	}

	ge_output * resource = (ge_output *)emalloc( sizeof( ge_output ) );

	resource->output = gumbo_parse( html );

	ZEND_REGISTER_RESOURCE( return_value, resource, le_gumbo_output );

//    GumboOutput * output = gumbo_parse( html );
//    gumbo_destroy_output( &kGumboDefaultOptions, output );
//    RETURN_STRINGL( html, htmlLength, 0 );

}
/* }}} */

/* {{{ php_gumbo_free_output */
static void php_gumbo_free_output( zend_rsrc_list_entry * resource TSRMLS_DC ) {
    
	ge_output * output = (ge_output *)resource->ptr;

	if ( !output ) {
        return;
    }

    gumbo_destroy_output( &kGumboDefaultOptions, output->output );

    efree( resource->ptr );
    resource->ptr = NULL;

}
/* }}} */

/* {{{ php_gumbo_free_node */
static void php_gumbo_free_node( zend_rsrc_list_entry * resource TSRMLS_DC ) {

    efree( resource->ptr );
    resource->ptr = NULL;

}
/* }}} */

/* {{{ proto string gumbo_destroy_output(resource resource) */
PHP_FUNCTION(gumbo_destroy_output)
{

	zval * resource;
	if ( zend_parse_parameters( ZEND_NUM_ARGS() TSRMLS_CC, "r", &resource ) == FAILURE ) {
		return;
	}

	ge_output * output = NULL;
	ZEND_FETCH_RESOURCE( output, ge_output *, &resource, -1, le_gumbo_output_name, le_gumbo_output );

//    gumbo_destroy_output( &kGumboDefaultOptions, output->output );
//    output->output = NULL;

	zend_list_delete( Z_LVAL_P( resource ) );

}
/* }}} */

/* {{{ proto string gumbo_output_get_root(resource output) */
PHP_FUNCTION(gumbo_output_get_root)
{

	zval * resource;
	if ( zend_parse_parameters( ZEND_NUM_ARGS() TSRMLS_CC, "r", &resource ) == FAILURE ) {
		return;
	}

	ge_output * output = NULL;
	ZEND_FETCH_RESOURCE( output, ge_output *, &resource, -1, le_gumbo_output_name, le_gumbo_output );

	ge_node * node = (ge_node *)emalloc( sizeof( ge_node ) );
	node->node = output->output->root;

	ZEND_REGISTER_RESOURCE( return_value, node, le_gumbo_node );

}
/* }}} */

/* {{{ proto string gumbo_node_get_type(resource node) */
PHP_FUNCTION(gumbo_node_get_type)
{

	zval * resource;
	if ( zend_parse_parameters( ZEND_NUM_ARGS() TSRMLS_CC, "r", &resource ) == FAILURE ) {
		return;
	}

	ge_node * node = NULL;
	ZEND_FETCH_RESOURCE( node, ge_node *, &resource, -1, le_gumbo_node_name, le_gumbo_node );

    RETURN_LONG( node->node->type );

}
/* }}} */

/* {{{ proto string gumbo_element_get_tag_name(resource node) */
PHP_FUNCTION(gumbo_element_get_tag_name) {

	zval * resource;
	if ( zend_parse_parameters( ZEND_NUM_ARGS() TSRMLS_CC, "r", &resource ) == FAILURE ) {
		return;
	}

	ge_node * node = NULL;
	ZEND_FETCH_RESOURCE( node, ge_node *, &resource, -1, le_gumbo_node_name, le_gumbo_node );

    if ( node->node->type != GUMBO_NODE_ELEMENT ) {
        RETURN_FALSE;
    }

    RETURN_STRING( gumbo_normalized_tagname( node->node->v.element.tag ), 1 );

}
/* }}} */

/* {{{ proto string gumbo_element_get_children(resource node) */
PHP_FUNCTION(gumbo_element_get_children) {

	zval * resource;
	if ( zend_parse_parameters( ZEND_NUM_ARGS() TSRMLS_CC, "r", &resource ) == FAILURE ) {
		return;
	}

	ge_node * node = NULL;
	ZEND_FETCH_RESOURCE( node, ge_node *, &resource, -1, le_gumbo_node_name, le_gumbo_node );

    if ( node->node->type != GUMBO_NODE_ELEMENT ) {
        RETURN_FALSE;
    }
    
    zval * array = NULL;
    ALLOC_INIT_ZVAL( array );
    array_init( array );
    
    GumboVector * children = &node->node->v.element.children;
    for ( int i = 0; i < children->length; i++ ) {

        ge_node * childNode = (ge_node *)emalloc( sizeof( ge_node ) );
        childNode->node = children->data[ i ];
        
        int childResource;
        ZEND_REGISTER_RESOURCE( (zval *)&childResource, childNode, le_gumbo_node );

        add_next_index_resource( array, childResource );

    }

    RETURN_ZVAL( array, 0, 0 );

}
/* }}} */

/* {{{ proto string gumbo_text_get_text(resource node) */
PHP_FUNCTION(gumbo_text_get_text) {

	zval * resource;
	if ( zend_parse_parameters( ZEND_NUM_ARGS() TSRMLS_CC, "r", &resource ) == FAILURE ) {
		return;
	}

	ge_node * node = NULL;
	ZEND_FETCH_RESOURCE( node, ge_node *, &resource, -1, le_gumbo_node_name, le_gumbo_node );

    if ( node->node->type != GUMBO_NODE_TEXT ) {
        RETURN_FALSE;
    }

    RETURN_STRING( node->node->v.text.text, 1 );

}
/* }}} */

/* {{{ proto string gumbo_element_get_attributes(resource node) */
PHP_FUNCTION(gumbo_element_get_attributes) {

	zval * resource;
	if ( zend_parse_parameters( ZEND_NUM_ARGS() TSRMLS_CC, "r", &resource ) == FAILURE ) {
		return;
	}

	ge_node * node = NULL;
	ZEND_FETCH_RESOURCE( node, ge_node *, &resource, -1, le_gumbo_node_name, le_gumbo_node );

    if ( node->node->type != GUMBO_NODE_ELEMENT ) {
        RETURN_FALSE;
    }

    zval * array = NULL;
    ALLOC_INIT_ZVAL( array );
    array_init( array );

    GumboVector * attributes = &node->node->v.element.attributes;
    for ( int i = 0; i < attributes->length; i++ ) {

        GumboAttribute * attribute = attributes->data[ i ];

        add_assoc_string( array, attribute->name, (char *)attribute->value, 1 );

    }

    RETURN_ZVAL( array, 0, 0 );

}
/* }}} */

/* {{{ proto string gumbo_element_get_tag_open(resource node) */
PHP_FUNCTION(gumbo_element_get_tag_open) {

	zval * resource;
	if ( zend_parse_parameters( ZEND_NUM_ARGS() TSRMLS_CC, "r", &resource ) == FAILURE ) {
		return;
	}

	ge_node * node = NULL;
	ZEND_FETCH_RESOURCE( node, ge_node *, &resource, -1, le_gumbo_node_name, le_gumbo_node );

    if ( node->node->type != GUMBO_NODE_ELEMENT ) {
        RETURN_FALSE;
    }

    GumboElement * element = &node->node->v.element;
    GumboStringPiece * openTag = &element->original_tag;
    
    RETURN_STRINGL( openTag->data, openTag->length, 1 );

}
/* }}} */

/* {{{ proto string gumbo_element_get_tag_close(resource node) */
PHP_FUNCTION(gumbo_element_get_tag_close) {

	zval * resource;
	if ( zend_parse_parameters( ZEND_NUM_ARGS() TSRMLS_CC, "r", &resource ) == FAILURE ) {
		return;
	}

	ge_node * node = NULL;
	ZEND_FETCH_RESOURCE( node, ge_node *, &resource, -1, le_gumbo_node_name, le_gumbo_node );

    if ( node->node->type != GUMBO_NODE_ELEMENT ) {
        RETURN_FALSE;
    }

    GumboElement * element = &node->node->v.element;
    GumboStringPiece * closeTag = &element->original_end_tag;

    RETURN_STRINGL( closeTag->data, closeTag->length, 1 );

}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
