# So, I made radix trees that can be traversed using `fseek`. 

This is broken into two main tools, a radix tree file writer and a radix tree file reader

I describe the format for the radix tree (.rdt) file here.

The radix tree files consists of a series of radix trees node followed by a series of data nodes.

A radix tree node is of the following format


``` 
  4 bytes         4 bytes        4 bytes       4 bytes       (key bit count + 7) / 8 bytes
[left offset] [right offset] [value offset] [key bit count] [key bits]
```

`left offset` and `right offset` are file offsets to the node's left and right children respectively, offset from `SEEK_SET`.
`value offset` is the file offset from `SEEK_SET` to the beginning of the data node, or all 0's if there is no value in that node.
`key bit count` is the number of bits packed into the radix tree node, which are stored in the bytes that follow it in `key bits`, with the lowest order bits coming first.
The root node is always the first node in the file.

A data node is of the following format
```
  4 bytes     data size bytes
[ data size ] [ data ]
```

This is simply the size of the data followed by the data.

The 4-byte integers are all unsigned `uint32_t`'s. Their byte order is currently not explicitly defined in the .rdt format and depends on the machine that generated it.

# Example usage
In this respository, I have used these trees to implement `bin/encode` and `bin/search`. `bin/encode` will use a modified form of JMDict, `parsed.txt`, to construct a radix tree file mapping Japanese words to their definitions. `bin/search` will, given the radix tree file and a japanese word as the first and second command line arguments respectively, search the radix tree file for the definitions and print them. If no second argument is passed in, it will read words from stdin.
```
svampis@thinkcentre:~/Radix-Tree-Files$ ./bin/encode
svampis@thinkcentre:~/Radix-Tree-Files$ ./bin/search jmdict.rdt おはよう
good morning    おはよう        おはよー
svampis@thinkcentre:~/Radix-Tree-Files$ ./bin/search jmdict.rdt 米航空宇宙局
National Aeronautics and Space Administration   NASA    べいこうくううちゅうきょく
svampis@thinkcentre:~/Radix-Tree-Files$
```

I have also implemented a few facilities on top of `bin/search` using Bash scripts. One of these is `examples/quiz.sh`, which, when given the JMDict rdt file and a list of Japanese words, will effectively function as a kanji reading quiz, printing the word and prompting the user to enter the word in kana.

```
svampis@thinkcentre:~/Radix-Tree-Files$ ./examples/quiz.sh jmdict.rdt ./examples/words.txt
空港
くうこう
Correct
海岸
かいがん
Correct
計算機
けいさんぎ
Incorrect
calculator computer けいさんき
男子
だんし
Correct
涼む
すずむ
Correct
svampis@thinkcentre:~/Radix-Tree-Files$
```

I have also implemented a bash script that, given the rdt file and a japanese sentence, will try to break it into tokens by checking for longest matches using the return code of the bin/search program. This script tends to fail hard on sentences that contain inflected verb forms that are not present in JMDict in their inflected forms.
```
svampis@thinkcentre:~/Radix-Tree-Files$ ./examples/tokenize.sh jmdict.rdt 最高のおやつはアイスクリームです
最高
の
おやつ
は
アイスクリーム
です
svampis@thinkcentre:~/Radix-Tree-Files$
```

Of course, several other applications could be implemented as well. The tokenizer could, for example, be used in conjunction with bin/search to split a Japanese sentence up into tokens, and then print each token with its definition in a one-liner

```
svampis@thinkcentre:~/Radix-Tree-Files$ for token in $(./examples/tokenize.sh jmdict.rdt 最高のおやつはアイスクリームです)
> do
> echo $token
> ./bin/search jmdict.rdt $token
> echo "---------"
> done
最高
best    supreme wonderful       finest  highest maximum most    uppermost       supreme さいこう        サイコー
---------
の
indicates possessive    nominalizes verbs and adjectives        substitutes for "ga" in subordinate phrases     indicates a confident conclusion        indicates emotional emphasis    indicates question       の
plain   field   hidden (structural) member      wild    lacking a political post        の      や      ぬ
shaft (of an arrow; made of bamboo)     の
unit of measurement for cloth breadth (30-38 cm)        の
---------
おやつ
between-meal snack      mid-afternoon (around 3 o'clock) snack  afternoon refreshment   afternoon tea   おやつ
---------
は
feather plume   down    wing    blade (of a fan  propeller       etc.)  shuttlecock (in badminton)      shuttlecock (in hanetsuki)      arrow feathers  はね    は
tooth   teeth   tooth (of a comb         saw     etc.)  cog     support (of a geta)     は
edge (of a knife or sword)      blade   prong (of an electrical plug)   は
group   faction clique  coterie (political) faction     wing    camp    school (of thought       painting        etc.)  sect    denomination    は
leaf    blade (of grass)        (pine) needle   は
indicates sentence topic        indicates contrast with another option (stated or unstated)     adds emphasis   は
yes     indeed  well    ha!     what?   huh?    sigh    はあ    はー    は      はぁ    はあっ  はーッ  ハーッ
large fan-shaped object held by an attendant and used to conceal the face of a noble     etc.   さしは  さしば  えい    は
middle section of a song (in gagaku or noh)     は
supremacy (over a nation)       hegemony        domination      leadership      championship    victory は
counter for waves (of a repeated occurrence)    は
---------
アイスクリーム
ice cream       icecream        アイスクリーム
---------
です
be      is      です
spit (of land)  でず    です
---------
svampis@thinkcentre:~/Radix-Tree-Files$
```
