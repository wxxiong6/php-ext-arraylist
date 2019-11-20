/*
  +----------------------------------------------------------------------+
  | PHP Version 7   arraylist                                            |
  +----------------------------------------------------------------------+
  | Copyright (c) 2019-2023 Wxxiong                                      |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Wxxiong <wxxiong6@gmail.com>                                 |
  |                                                                      |
  +----------------------------------------------------------------------+
*/


#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "ext/standard/php_var.h"
#include "zend_exceptions.h"
#include "php_arraylist.h"

#ifndef ARRAY_LIST_SIZE
#define ARRAY_LIST_SIZE  8
#endif

/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif

zend_class_entry *array_list_ce;
zend_object_handlers handler_array_list;

typedef struct _arraylist { /* {{{ */
	size_t nSize;     
 	size_t nNumUsed;  
	size_t nNextIndex;   
	zval *elements;  
} arraylist;
/* }}} */

typedef struct _arraylist_object { /* {{{ */
	arraylist             array;
	zend_object            std;
} arraylist_object;
/* }}} */




static inline arraylist_object *arraylist_from_obj(zend_object *obj) /* {{{ */ {
	return (arraylist_object*)((char*)(obj) - XtOffsetOf(arraylist_object, std));
}
/* }}} */

#define Z_ARRAYLIST_P(zv)  arraylist_from_obj(Z_OBJ_P((zv)))


/* {{{ arginfo
 */
ZEND_BEGIN_ARG_INFO_EX(arginfo_arraylist__construct, 0, 0, 1)
	ZEND_ARG_INFO(0, size)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_arraylist_add, 0, 0, 1)
	ZEND_ARG_INFO(0, val)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_arraylist_get, 0, 0, 1)
	ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_arraylist_void, 0)
ZEND_END_ARG_INFO()
/* }}} */

static void arraylist_init(arraylist *array, zend_long size) /* {{{ */
{
	array->elements = NULL;
	array->elements = (zval *)ecalloc(size, sizeof(zval));
    array->nNumUsed = 0;
	array->nNextIndex = 0;
	array->nSize = size;
}
/* }}} */

/* {{{ void arraylist::__construct()
 */
PHP_METHOD(arraylist, __construct)
{
	zend_long size = ARRAY_LIST_SIZE;
	zval *object = getThis();
	arraylist_object *intern;
	intern = Z_ARRAYLIST_P(object);

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(size)
	ZEND_PARSE_PARAMETERS_END();

	arraylist_init(&intern->array, size);
	
}
/* }}} */

static void arraylist_resize(arraylist *array) /* {{{ */
{
	if (array->nNumUsed == array->nSize)
	{
        size_t i = 0;
		size_t oldSize = array->nSize == 1? 2 : array->nSize;
		size_t newSize = oldSize + (oldSize >> 1);
		zval *elements  = (zval *)ecalloc(newSize, sizeof(zval));
		for (; i < array->nSize; i++)
		{
			elements[i] = array->elements[i];
            zval_dtor(&array->elements[i]);
		}
		efree(array->elements);
		array->elements = NULL;
		array->elements = elements;
        // Z_TRY_ADDREF(elements);
		array->nSize = newSize;
	}
}
/* }}} */

static void arraylist_add(arraylist *array, zval *val) /* {{{ */
{
	if (array == NULL)
	{
		 php_error_docref(NULL, E_NOTICE, "array cann't be empty.");
		 return;
	} 
    // arraylist_resize(array);
    // php_printf("nNextIndex=%zu,nSize=%zu \n", array->nNextIndex, array->nSize);
	// php_debug_zval_dump(&array->elements[array->nNextIndex], 1);
	Z_TRY_ADDREF(array->elements[array->nNextIndex]);
	ZVAL_ZVAL(&array->elements[array->nNextIndex], val, 1, 1);
	
	array->nNextIndex++;
	array->nNumUsed++;
}


/* {{{ string arraylist::add( [  $val ] )
 */
PHP_METHOD(arraylist, add)
{
	zval *object = getThis();
	arraylist_object *intern;
    zval *val;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(val)
	ZEND_PARSE_PARAMETERS_END();

    
	intern = Z_ARRAYLIST_P(object);

	arraylist_add(&intern->array, val);
	
	RETURN_TRUE;
}

/* {{{ zval arraylist::get(int $i)
 */
PHP_METHOD(arraylist, get)
{
	zval *object = getThis();
	arraylist_object *intern;
    zend_long key;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(key)
	ZEND_PARSE_PARAMETERS_END();

	intern = Z_ARRAYLIST_P(object);

	if (key >= intern->array.nSize)
	{
        php_error_docref(NULL, E_NOTICE, "Undefined offset:%ld", key);
        RETURN_NULL();
    }

    if (&intern->array.elements[key] != NULL) 
    {
		Z_TRY_ADDREF(intern->array.elements[key]);
        RETURN_ZVAL(&intern->array.elements[key], 1, 0);
    } else {
        RETURN_NULL();
    }
} 


PHP_METHOD(arraylist, count)
{
	zval *object = getThis();
	arraylist_object *intern;
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	intern = Z_ARRAYLIST_P(object);
	RETURN_LONG(intern->array.nNumUsed);
}

PHP_METHOD(arraylist, getSize)
{
	zval *object = getThis();
	arraylist_object *intern;
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	intern = Z_ARRAYLIST_P(object);
	RETURN_LONG(intern->array.nSize);
}

/* {{{ proto object arraylist::toArray()
*/
PHP_METHOD(arraylist, toArray)
{
	arraylist_object *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	intern = Z_ARRAYLIST_P(getThis());
	if (intern->array.nNumUsed > 0) {
		int i = 0;
		array_init_size(return_value, intern->array.nNumUsed);
		for (; i < intern->array.nNumUsed; i++) {
			if (!Z_ISUNDEF(intern->array.elements[i])) {
				zend_hash_index_update(Z_ARRVAL_P(return_value), i, &intern->array.elements[i]);
				Z_TRY_ADDREF(intern->array.elements[i]);
			} else {
				zend_hash_index_update(Z_ARRVAL_P(return_value), i, &EG(uninitialized_zval));
			}
		}
	} else {
		RETURN_NULL();
	}
}
/* }}} */


static void arraylist_destruct(arraylist_object *intern)/* {{{ */
{
	// arraylist_object *intern = arraylist_from_obj(object);
	arraylist *array = &intern->array;
	if (array->elements)
	{
		int i = 0;
		for (; i < array->nNumUsed; i++) 
		{
             if (&array->elements[i]) {
                zval_ptr_dtor(&array->elements[i]);
             }
		}
		efree(array->elements);
		array->elements = NULL;
	    array->nNumUsed = 0;
		array->nSize = 0;
	    array->nNextIndex = 0;
	}
  
}
/* }}} */

PHP_METHOD(arraylist, __destruct)
{
	zval *object = getThis();
	arraylist_object *intern;
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	intern = Z_ARRAYLIST_P(object);
	arraylist_destruct(intern);
}

/* {{{ arraylist_functions[] 扩展函数
 */
static const zend_function_entry arraylist_functions[] = {
	PHP_FE_END
};
/* }}} */

/* {{{ arraylist__methods[] 扩展类方法
 */
static const zend_function_entry arraylist_methods[] = { 
	PHP_ME(arraylist, __construct,	arginfo_arraylist__construct, ZEND_ACC_PUBLIC)
	PHP_ME(arraylist, add,          arginfo_arraylist_add,        ZEND_ACC_PUBLIC)
	PHP_ME(arraylist, get,          arginfo_arraylist_get,        ZEND_ACC_PUBLIC)
	PHP_ME(arraylist, count,        arginfo_arraylist_void,       ZEND_ACC_PUBLIC)
	PHP_ME(arraylist, toArray,      arginfo_arraylist_void,       ZEND_ACC_PUBLIC)
	PHP_ME(arraylist, getSize,      arginfo_arraylist_void,       ZEND_ACC_PUBLIC)
	PHP_ME(arraylist, __destruct,   arginfo_arraylist_void,       ZEND_ACC_PUBLIC)
	PHP_FE_END
};
/* }}} */

static void arraylist_object_free_storage(zend_object *object)/* {{{ */
{
	arraylist_object *intern = arraylist_from_obj(object);
	arraylist *array = &intern->array;
	arraylist_destruct(intern);
	zend_object_std_dtor(&intern->std);
}
/* }}} */


/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(arraylist)
{
	return SUCCESS;
}
/* }}} */


/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(arraylist)
{
	return SUCCESS;
}


/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(arraylist)
{
#if defined(ZTS) && defined(COMPILE_DL_ARRAYLIST)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION  phpinfo显示扩展信息
 */
PHP_MINFO_FUNCTION(arraylist)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "arraylist support", "enabled");
	php_info_print_table_row(2, "Version", PHP_ARRAYLIST_VERSION);
	php_info_print_table_end();
}
/* }}} */

// 模块初始化时调用
PHP_MINIT_FUNCTION(arraylist) /* {{{ */ {
    zend_class_entry ce;
	//REGISTER_INI_ENTRIES(); // 注册ini

	//注册常量
	//第一个参数是常量的名字,
	//第二个参数来表明长度
	//CONST_CS标识是否大小写敏感
	REGISTER_STRINGL_CONSTANT(
		"ARRAYLIST_VERSION", 
		PHP_ARRAYLIST_VERSION, 
		sizeof(PHP_ARRAYLIST_VERSION)-1, 
		CONST_CS|CONST_PERSISTENT
	);
	INIT_CLASS_ENTRY(ce, "ArrayList", arraylist_methods); //注册类及类方法
    array_list_ce = zend_register_internal_class(&ce);

    memcpy(&handler_array_list, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	handler_array_list.offset          = XtOffsetOf(arraylist_object, std);
	
	handler_array_list.clone_obj       = zend_objects_clone_obj;
	handler_array_list.free_obj        = arraylist_object_free_storage;
	handler_array_list.dtor_obj        = zend_objects_destroy_object;
    return SUCCESS;
}
/* }}} */

/* {{{ arraylist_module_entry
 */
zend_module_entry arraylist_module_entry = { 
	STANDARD_MODULE_HEADER,
	"arraylist",					/* 扩展名 name */
	arraylist_functions,			/* 注册函数，注意这里不是注册方法 */
	PHP_MINIT(arraylist),			/* PHP_MINIT - Module initialization */
	PHP_MSHUTDOWN(arraylist),		/* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(arraylist),			/* PHP_RINIT - Request initialization */
	PHP_RSHUTDOWN(arraylist),		/* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(arraylist),			/*  - Module info */
	PHP_ARRAYLIST_VERSION,		    /* Version */
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_ARRAYLIST
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(arraylist)
#endif
