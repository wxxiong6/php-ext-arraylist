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
$ret->toArray();
```
二维ArrayList
```php
$ret = new ArrayList(10);
$ret2 = new ArrayList(3);
$ret2->add(1);
$ret2->add(2);
$ret2->add(3);
$ret->add($ret2);
for ($i=0; $i<$ret->count(); $i++) {
    for ($j=0; $j<$ret2->count(); $j++) {
        var_dump($ret2->get($j));
    }
}
```
ArrayList存array 
```php
$ret3 = new ArrayList(1);
$ret3->add([1, 3,4,5, 6,]);
$ret3->add([3, 4,5,6, 7,]);
for ($i=0; $i<$ret3->count(); $i++){
	foreach($ret3->get($i) as $v){
		var_dump($v);
	}
}
```
