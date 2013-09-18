--TEST--
Check parse
--FILE--
<?php
$output = gumbo_parse( "<html><body><p>Hello World</p></body></html>" );
if ( $output ) {
    echo "1";
}
?>
--EXPECT--
1