# php-ext-arraylist
php扩展ArrayList

## linux mac 环境下编译安装
```shell
phpize
./configure --enable-arraylist
make 
make install
```

支持函数
```php
$ret = new ArrayList();
$ret->add(1);
$ret->get(0);
$ret->count();
$ret->getSize();
$res->toArray();
```
