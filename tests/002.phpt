--TEST--
Check for arraylist presence
--SKIPIF--
<?php if (!extension_loaded("arraylist")) print "skip"; ?>
--FILE--
<?php
echo "arraylist extension is available\n";
$ret = new ArrayList();
$ret->add(1);
var_dump($ret->get(0));
var_dump($ret->count());
var_dump($ret->getSize());
var_dump(count($ret->toArray()));
?>
--EXPECT--
arraylist extension is available
int(1)
int(1)
int(8)
int(1)