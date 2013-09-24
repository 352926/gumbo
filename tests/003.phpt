--TEST--
Get open and closing tags
--FILE--
<?php

$output = gumbo_parse( "<html><body><p class=\"big\">Hello World</p></body></html>" );
$rootNode = gumbo_output_get_root( $output );
$getElementsByTagName = function( $tagName, $node ) use ( &$getElementsByTagName ) {
    $elements = [];
    switch ( gumbo_node_get_type( $node ) ) {
        case GUMBO_NODE_ELEMENT:
            if ( gumbo_element_get_tag_name( $node ) == $tagName ) {
                array_push( $elements, $node );
            }
            foreach ( gumbo_element_get_children( $node ) as $childNode ) {
                $elements = array_merge( $elements, $getElementsByTagName( $tagName, $childNode ) );
            }
            break;
    }
    return $elements;
};

$elements = $getElementsByTagName( "p", $rootNode );
if ( count( $elements ) == 1 ) {
    echo gumbo_element_get_tag_open( $elements[ 0 ] );
    echo gumbo_element_get_tag_close( $elements[ 0 ] );
}

?>
--EXPECT--
<p class="big"></p>