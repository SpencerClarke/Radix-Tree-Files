# So, I made radix trees that can be traversed using `fseek`. 

This is broken into two main tools, a radix tree file writer and a radix tree file reader

I describe the format for the radix tree (.rdt) file here.

The radix tree files consists of a series of radix tree node followed by a series of data nodes.

A radix tree node is of the following format


``` 
  4 bytes         4 bytes        4 bytes       4 bytes       (key bit count + 7) / 8 bytes
[left offset] [right offset] [value offset] [key bit count] [key bit]
```

`left offset` and `right offset` are file offsets to the node's left and right children respectively, offset from `SEEK_SET`.
`value offset` is the file offset from `SEEK_SET` to the beginning of the data node, or all 0's if there is no value in that node.
`key bit count` is the number of bits packed into the radix tree node, which are stored in the bytes that follow it, with the lowest order bits coming first.
The root node is always the first node in the file.

A data node is of the following format
```
  4 bytes     data size bytes
[ data size ] [ data ]
```

This is simply the size of the data followed by the data.

The 4-byte integers are all unsigned `uint32_t`'s. Their byte order is currently not explicitly defined in the .rdt format and depends on the machine that generated it.

# Example usage
In this respository, I have used these trees to implement `bin/encode` and `bin/search`. `bin/encode` will use a modified form of JMDict, `parsed.txt`, to construct a radix tree file mapping Japanese words to their definitions. `bin/search` will, given the radix tree file and a japanese word as the first and second command line arguments respectively, search the radix tree file for the definitions and print them.
