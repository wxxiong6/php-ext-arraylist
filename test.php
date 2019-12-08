<?php
ini_set("memory_limit", "-1");

for($size = 1000; $size < 5000000; $size *= 2) { 
    echo PHP_EOL , "Testing size: ", number_format($size), PHP_EOL; 
    for($m = memory_get_usage(), $s = microtime(true), $container = Array(), $i = 0; $i < $size; $i++) $container[$i] = "$i"; 
    echo "Array():         " , "time:", (microtime(true) - $s) , " memory:", (memory_get_usage() - $m)/1024, PHP_EOL; 
    unset($container);
    for($m = memory_get_usage(), $s = microtime(true), $container = new ArrayList($size), $i = 0; $i < $size; $i++) $container->add("$i"); 
    echo "ArrayList():     " , "time:", (microtime(true) - $s) , " memory:", (memory_get_usage() - $m)/1024, PHP_EOL; 
    unset($container);
    for($m = memory_get_usage(), $s = microtime(true), $container = new SplFixedArray($size), $i = 0; $i < $size; $i++) $container[$i] = "$i"; 
    echo "SplFixedArray(): "  , "time:", (microtime(true) - $s) , " memory:", (memory_get_usage() - $m)/1024, PHP_EOL;
} 
?> 