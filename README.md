Printed math expression parser
==============================

Here you can find a pilot software for recognition of printed
mathematical expressions. It corresponds to the description presented
in the following publication:

 - Francisco Álvaro, Joan-Andreu Sánchez, José-Miguel Benedí.
   *Recognition of Printed Mathematical Expressions Using
    Two-dimensional Stochastic Context-Free Grammars*.
   International Conference on Document Analysis and Recognition (ICDAR), 2011.

The software is able to parse most of image formats, thanks to the
ImageMagick interface. It provides the recognition output in LaTeX format.


License
-------
This software is released under the [GNU General Public License
version 3.0 (GPLv3)] [1]


Instructions
------------
The parser is written in C++ and it requries the [Magick++ library] [2],
g++ and a Makefile to compile it. Once the requirements are available, you
can proceed with the installation as follows:

 1. Obtain the package using git:

        $ git clone https://github.com/falvaro/pme_parser.git

    Or [download it as a zip file] [3]

 2. Go to the directory containing the source code.

 3. Compile the parser

        $ make

As a result, you will have the executable file "*parser*" ready to
recognize printed math expressions. A sample grammar and symbol dataset
for classification are provided.

For instance, for recognizing the sample expression of image exp1.png

![math expression sample](https://github.com/falvaro/pme_parser/tree/SampleExps/exp1.png "exp1.png")

you only have to use the following command:

        $ ./parser SampleGrammar/math.gram SampleExps/exp1.png

that will provide many information such that the last line is the LaTeX
transcription of the recognized math expression

        $ LaTeX: {x}^{2} + {y}_{1} + \sqrt{3}



Citations
---------
If you use this software for your research, please cite the following reference:

<pre>
@INPROCEEDINGS{falvaro11,
 author       = {Francisco \'Alvaro and Joan-Andreu Sánchez and José-Miguel Benedí},
 title        = {Recognition of Printed Mathematical Expressions Using Two-dimensional Stochastic Context-Free Grammars},
 booktitle    = {International Conference on Document Analysis and Recognition (ICDAR)},
 year         = {2011},
 pages        = {1225-1229},
}
</pre>




[1]: http://www.gnu.org/licenses/gpl-3.0.html
[2]: http://www.imagemagick.org/Magick++
[3]: https://github.com/falvaro/pme_parser/archive/master.zip
