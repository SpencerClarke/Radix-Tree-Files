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

```
svampis@thinkcentre:~/Radix-Tree-Files$ ./bin/encode
svampis@thinkcentre:~/Radix-Tree-Files$ ./bin/search jmdict.rdt おはよう
good morning    おはよう        おはよー
svampis@thinkcentre:~/Radix-Tree-Files$ ./bin/search jmdict.rdt 米航空宇宙局
National Aeronautics and Space Administration   NASA    べいこうくううちゅうきょく
svampis@thinkcentre:~/Radix-Tree-Files$
```

I also tried implementing a program that, given a japanese sentence, will break it into words by matching longest suffixes. It only works when the lexeme is contained in JMDict, so it won't work great for sentences with conjugated verbs in them

```
svampis@thinkcentre:~/Radix-Tree-Files$ ./bin/tokenize jmdict.rdt 学校に行くのは最悪だね
学校
に
行く
の
は
最悪
だ
ね
svampis@thinkcentre:~/Radix-Tree-Files$ ./bin/tokenize jmdict.rdt 俺の好きな食べ物はハンバーガーだ
俺
の
好き
な
食べ物
は
ハンバーガー
だ
svampis@thinkcentre:~/Radix-Tree-Files$
```


So, Why would someone want this?
Well, it provides O(k) lookup times without loading a trie or a hashmap into main memory where k is key length.
You can perform somewhat fast lookups, you use very little RAM used while searching, and 0 RAM used while not searching.
