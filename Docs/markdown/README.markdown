% Quick Introduction to Markdown
% Niraj Dudani
% January 2, 2013

# Introduction

This document is written in Markdown[^markdown]. Here is the philosophy behind
this format:

> A Markdown-formatted document should be publishable as-is, as plain text, 
> without looking like it’s been marked up with tags or formatting 
> instructions. 

For MOOSE documentation, we use Pandoc[^pandoc] to convert from Markdown to
HTML. Pandoc adds some nice things to Markdown, like tables and footnotes.

# Converting to HTML

To convert this document to HTML, run the command in the `build` script included
in this directory. This command also requests `pandoc` to generate a
table-of-contents for this document.

To install `pandoc` on Ubuntu, simply issue:

    sudo apt-get install pandoc

# Examples

In the following subsections, we look at a few examples.

## Lists, emphasis, etc.

This is a bullet-list of some things that Markdown can do:

  - This is *emphasis*.

  - This is **stronger emphasis**.

  - You can add ^superscript^ / ~subscript~ / ~~strikeout~~.

  - You can add `verbatim text` inline. This is useful for `variables`,
    `functions()`, etc.

## Quoting text, code, etc.

To quote text, use `>`, as in email:

> **Rock Story**  
> \ -- *Dik Browne*  
> 
> LE: What crazy mixed-up rocks!  
> HH: Quiet, stupid!  
> 
> HH: You're in a very special place... Full of age and mystery...  
> LE: No kidding!  
> 
> LE: **Wow! Crazy!** What is it?  
> HH: It's a monument!  
> 
> HH: Thousands of people slaved for years to drag those stones here and put
>     them in place!  
> LE: Why?  
> 
> HH: For their leader! When you're a big shot, you do that so people will
>     always remember you!  
> \ \ \ \ That's called **immortality!**  
> LE: **Wow!**  
> 
> LE: Who was he?  
> HH: Nobody knows...

 If you're lazy, a single `>` is enough:

> **The Purist**  
\ \ -- *Ogden Nash*  
\
I give you now Professor Twist,  
A conscientious scientist,  
Trustees exclaimed, "He never bungles!"  
And sent him off to distant jungles.  
Camped on a tropic riverside,  
One day he missed his loving bride.  
She had, the guide informed him later,  
Been eaten by an alligator.  
Professor Twist could not but smile.  
"You mean," he said, "a crocodile."  

At any time,  
end lines with  
2 spaces  
to retain  
line-endings (as done in the examples above).

Insert code using 4 spaces:

    echo "Sanitizing..."
    rm -rf /

or a few tildes:

~~~~~~~~~~{.python .numberLines startFrom="10"}
def factorial( n ):
    if ( n <= 1 ):
        return 1
    else:
        return n * factorial( n - 1 )
~~~~~~~~~~

## Linking, images and tables

You can link to [an external website](http://www.zombo.com/), or to
[a section](#introduction) on the same page.

You can insert images and have captions for them. Here an image has been
sandwiched between 2 horizontal rules:

**********

![*Purkinje cell in MOOSE*](images/purkinje.png "You can add alt-text too!")

**********

Tables can have headers:

  Right     Left     Center     Default
-------     ------ ----------   -------
     12     12        12            12
    123     123       123          123
      1     1          1             1

or not:

----- ----- ----- ----- ----- ----- ----- -----
  ♜     ♞     ♝     ♛     ♚     ♝     ♞     ♜
  ♟     ♟     ♟     ♟     ♟     ♟     ♟     ♟
  ◻     ◼     ◻     ◼     ◻     ◼     ◻     ◼
  ◼     ◻     ◼     ◻     ◼     ◻     ◼     ◻
  ◻     ◼     ◻     ◼     ◻     ◼     ◻     ◼
  ◼     ◻     ◼     ◻     ◼     ◻     ◼     ◻
  ♙     ♙     ♙     ♙     ♙     ♙     ♙     ♙
  ♖﻿     ♘     ♗     ♕     ♔     ♗     ♘     ♖
----- ----- ----- ----- ----- ----- ----- -----

# Conclusion

Markdown and Pandoc have many more features. For these, go to the links
in the footnotes.

# Footnotes

[^markdown]: [Link to Markdown website](http://daringfireball.net/projects/markdown/basics)
[^pandoc]: [Link to Pandoc website](http://johnmacfarlane.net/pandoc/README.html)
