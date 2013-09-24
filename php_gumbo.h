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

#ifndef PHP_GUMBO_H
#define PHP_GUMBO_H

extern zend_module_entry gumbo_module_entry;
#define phpext_gumbo_ptr &gumbo_module_entry

#ifdef PHP_WIN32
#	define PHP_GUMBO_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_GUMBO_API __attribute__ ((visibility("default")))
#else
#	define PHP_GUMBO_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(gumbo);
PHP_MSHUTDOWN_FUNCTION(gumbo);
PHP_RINIT_FUNCTION(gumbo);
PHP_RSHUTDOWN_FUNCTION(gumbo);
PHP_MINFO_FUNCTION(gumbo);

PHP_FUNCTION(gumbo_parse);
PHP_FUNCTION(gumbo_destroy_output);
PHP_FUNCTION(gumbo_output_get_root);
PHP_FUNCTION(gumbo_node_get_type);
PHP_FUNCTION(gumbo_element_get_tag_name);
PHP_FUNCTION(gumbo_element_get_children);
PHP_FUNCTION(gumbo_text_get_text);
PHP_FUNCTION(gumbo_element_get_attributes);
PHP_FUNCTION(gumbo_element_get_tag_open);
PHP_FUNCTION(gumbo_element_get_tag_close);

/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:     

ZEND_BEGIN_MODULE_GLOBALS(gumbo)
	long  global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(gumbo)
*/

/* In every utility function you add that needs to use variables 
   in php_gumbo_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as GUMBO_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#include "gumbo.h"

typedef struct _ge_output {
    GumboOutput * output;
} ge_output;

typedef struct _ge_node {
    GumboNode * node;
} ge_node;

#ifdef ZTS
#define GUMBO_G(v) TSRMG(gumbo_globals_id, zend_gumbo_globals *, v)
#else
#define GUMBO_G(v) (gumbo_globals.v)
#endif

#endif	/* PHP_GUMBO_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
