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


#ifndef PHP_ARRAYLIST_H
# define PHP_ARRAYLIST_H

extern zend_module_entry arraylist_module_entry;
# define phpext_arraylist_ptr &arraylist_module_entry

# define PHP_ARRAYLIST_VERSION "0.1.0"

#ifndef PHP_FE_END
#define PHP_FE_END { NULL, NULL, NULL }
#endif

# if defined(ZTS) && defined(COMPILE_DL_ARRAYLIST)
ZEND_TSRMLS_CACHE_EXTERN()
# endif
PHP_MINIT_FUNCTION(arraylist);



#endif  /* PHP_ARRAYLIST_H */
