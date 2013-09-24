<?php

$html = "<html><body><p>Hello World</p></body></html>";
echo "html is: $html\n";
$output = gumbo_parse( $html );
var_dump( $output );
$root = gumbo_output_get_root( $output );

function dig( $node ) {
    var_dump( $node );
    $nodeType = gumbo_node_get_type( $node );
    switch ( $nodeType ) {
        case GUMBO_NODE_ELEMENT: {
            var_dump( gumbo_element_get_tag_name( $node ) );
            $children = gumbo_element_get_children( $node );
            var_dump( $children );
            foreach ( $children as $child ) {
                dig( $child );
            }
            break;
        }
        case GUMBO_NODE_TEXT: {
            var_dump( gumbo_text_get_text( $node ) );
            break;
        }
    }
    
}

dig( $root );

gumbo_destroy_output( $output );

$html = "<html><body><p>Hello World of <strong>PAIN</strong></p></body></html>";
$output = gumbo_parse( $html );
$root = gumbo_output_get_root( $output );

function getTextContent( $node ) {
    $textContent = "";
    $nodeType = gumbo_node_get_type( $node );
    switch ( $nodeType ) {
        case GUMBO_NODE_ELEMENT: {
            $tag = gumbo_element_get_tag_name( $node );
            if ( in_array( $tag, [ 'script', 'style' ] ) ) {
                break;
            }
            $children = gumbo_element_get_children( $node );
            foreach ( $children as $child ) {
                $textContent .= getTextContent( $child );
            }
            break;
        }
        case GUMBO_NODE_TEXT: {
            $text = gumbo_text_get_text( $node );
            $textContent .= strlen( $text ) ? $text : " ";
            break;
        }
    }
    
    return $textContent;

}

$textContent = getTextContent( $root );
echo "\n";
echo "html is: $html\n";
echo "textContent is: $textContent\n";
echo "\n";

gumbo_destroy_output( $output );

$html = "<html><body><p>Hello World of <strong>PAIN</strong><style>body{background:black}</style></p></body></html>";
$output = gumbo_parse( $html );
$root = gumbo_output_get_root( $output );

$textContent = getTextContent( $root );
echo "\n";
echo "html is: $html\n";
echo "textContent is: $textContent\n";
echo "\n";

gumbo_destroy_output( $output );

$html = "<html><body><div class=\"important\"></div></body></html>";
echo "\n";
echo "html is: $html\n";
$output = gumbo_parse( $html );
$root = gumbo_output_get_root( $output );
foreach ( gumbo_element_get_children( $root ) as $child ) {
    foreach ( gumbo_element_get_children( $child ) as $child2 ) {
        echo "attributes are:\n";
        var_dump( gumbo_element_get_attributes( $child2 ) );
    }
}
echo "\n";
gumbo_destroy_output( $output );

$html = "<html><body><p>Hello World</p></body></html>";
$output = gumbo_parse( $html );
$rootNode = gumbo_output_get_root( $output );

$getTextContent = function( $node ) use ( &$getTextContent ) {
    $textContent = "";
    switch ( gumbo_node_get_type( $node ) ) {
        case GUMBO_NODE_ELEMENT:
            foreach ( gumbo_element_get_children( $node ) as $childNode ) {
                $textContent .= $getTextContent( $childNode );
            }
            break;
        case GUMBO_NODE_TEXT:
            $textContent = gumbo_text_get_text( $node );
            break;
    }
    return $textContent;
};
echo $getTextContent( $rootNode );