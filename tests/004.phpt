--TEST--
Check for arraylist array 
--SKIPIF--
<?php if (!extension_loaded("arraylist")) print "skip"; ?>
--FILE--
<?php
$ret = new ArrayList();
$ret->add([1, 3, 4, 5, 6,]);
$ret->add([8, 9, 10]);
for ($i = 0; $i< $ret->count(); $i++) {
    foreach($ret->get($i) as $v){
        var_dump($v);
    }
}
?>
--EXPECT--
int(1)
int(3)
int(4)
int(5)
int(6)
int(8)
int(9)
int(10)