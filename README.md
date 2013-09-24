Gumbo
=====

Low Level PHP Extension for Gumbo HTML5 Parser (https://github.com/google/gumbo-parser)

We recommend you do NOT use this in production at this time - this is a super early release

Installation
=====

```bash
git clone https://github.com/BipSync/gumbo.git
cd gumbo
phpize
./configure
make
make install
```

This will build a 'gumbo.so' shared extension, load it in php.ini using:

```ini
[gumbo]
extension = gumbo.so
```

Usage
=====

Get the text of a html string:
```php
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
```

Returns:
```
Hello World
```

Functions
=========

Function|Returns
---|---
`gumbo_parse( $html )`|Gumbo Output Resource
`gumbo_output_get_root( $output )`|Gumbo Node Resource
`gumbo_node_get_type( $node )`|int (see constants)
`gumbo_element_get_tag_name( $elementNode )`|string
`gumbo_element_get_tag_open( $elementNode )`|string
`gumbo_element_get_tag_close( $elementNode )`|string
`gumbo_element_get_attributes( $elementNode )`|associative array
`gumbo_element_get_children( $elementNode )`|array of Gumbo Node Resources
`gumbo_text_get_text( $textNode )`|string
`gumbo_destroy_output( $output )`|

Constants
=========
* GUMBO_NODE_DOCUMENT
* GUMBO_NODE_ELEMENT
* GUMBO_NODE_TEXT
* GUMBO_NODE_CDATA
* GUMBO_NODE_COMMENT
* GUMBO_NODE_WHITESPACE


Contact
=======

If you have found a bug, have an idea or a question, email me at paul@bipsync.com
