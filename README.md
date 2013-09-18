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
```

This will build a 'gumbo.so' shared extension into the modules/ directory

php.ini:
```ini
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
        case 1: // element
            foreach ( gumbo_element_get_children( $node ) as $childNode ) {
                $textContent .= $getTextContent( $childNode );
            }
            break;
        case 2: // text
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
`gumbo_node_get_type( $node )`|int ( 0=document, 1=element, 2=text, 3=cdata, 4=comment, 5=whitespace )
`gumbo_element_get_tag( $elementNode )`|string
`gumbo_element_get_attributes( $elementNode )`|associative array
`gumbo_element_get_children( $elementNode )`|array of Gumbo Node Resources
`gumbo_text_get_text( $textNode )`|string
`gumbo_destroy_output( $output )`|


Contact
=======

If you have found a bug, have an idea or a question, email me at paul@bipsync.com
