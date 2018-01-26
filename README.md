# Sudoku Suite

I actually intended to implement a Sudoku generator, but it kind of
escalated into an attempt to produce a very fast Sudoku solver instead.

All utilities are designed to use standard streams (`stdin`, `stdout`, `stderr`)
to allow input/output redirection. Data can be piped from one utility
to another so that solving a single Sudoku instance is as easy as

    echo 030000000000195000008000060800060000400800001000020000060000280000419005000000070|zdss|spp bordered

The input is expected to consist of the digits 0-9, where 0 represents an empty
field, in row-major order, for a total of 81 digits per line.
Every line will be treated as a different puzzle instance, so you can pass
as many instances as you wish. Avoid blank lines however. Valid input example:

    030000000000195000008000060800060000400800001000020000060000280000419005000000070

The included text file should contain plenty of example instances.

#### Pending changes:
- further optimize solver
- implement instance generator


## Installation instructions

To build the solver, a simple

    make

should do the job. The output will be compiled into a directory called `build`
by default, which should be created automatically. If you wish to override the
target directory you can do so by running

    make build_dir=path/to/directory

instead. If for some reason `mkdir` is not working properly,
you can disable directory creation entirely by using

    make mkdir=#

Note that you will have to create the target directory yourself in this case.
You can also change the default compiler and compilation flags by setting the
`cc` and `cc_flags` variables. Use

    make clean

to remove all compiled files.


### Zero-delay Sudoku Solver (zdss)

The compilation of the main solver utility defaults to full optimization
which suppresses input validation. You can change this behavior by running

    make zdss_opt=

to clear the optimization flags. You can set additional compilation flags
for the solver by setting the `zdss_flags` variable. The solver also contains
a timer utility which outputs to `stderr`. The timer can be enabled by
defining the `TIMED` macro. It is possible do so without editing the
source code by executing

    make zdss_flags=-DTIMED


### Sudoku Pretty Printer (spp)

The printing utility does not support any compilation options at this time.
However, you are able to choose between 3 different printing styles:

#### Default style

The default style explicitly presents lines and blocks without visual clutter.

    echo 030000000000195000008000060800060000400800001000020000060000280000419005000000070|spp

produces the following output:

    . 3 .  . . .  . . .
    . . .  1 9 5  . . .
    . . 8  . . .  . 6 .

    8 . .  . 6 .  . . .
    4 . .  8 . .  . . 1
    . . .  . 2 .  . . .

    . 6 .  . . .  2 8 .
    . . .  4 1 9  . . 5
    . . .  . . .  . 7 .

#### Bordered style

This style is intended to improve readability for humans.

    echo 030000000000195000008000060800060000400800001000020000060000280000419005000000070|spp bordered

produces the following output:

    +-------+-------+-------+
    | . 3 . | . . . | . . . |
    | . . . | 1 9 5 | . . . |
    | . . 8 | . . . | . 6 . |
    +-------+-------+-------+
    | 8 . . | . 6 . | . . . |
    | 4 . . | 8 . . | . . 1 |
    | . . . | . 2 . | . . . |
    +-------+-------+-------+
    | . 6 . | . . . | 2 8 . |
    | . . . | 4 1 9 | . . 5 |
    | . . . | . . . | . 7 . |
    +-------+-------+-------+

#### Minimal style

This can be potentially useful if you wish to to convert to a different
representation, e.g. if an external program requires the rows to be separated.

    echo 030000000000195000008000060800060000400800001000020000060000280000419005000000070|spp minimal

produces the following output:

    0 3 0 0 0 0 0 0 0
    0 0 0 1 9 5 0 0 0
    0 0 8 0 0 0 0 6 0
    8 0 0 0 6 0 0 0 0
    4 0 0 8 0 0 0 0 1
    0 0 0 0 2 0 0 0 0
    0 6 0 0 0 0 2 8 0
    0 0 0 4 1 9 0 0 5
    0 0 0 0 0 0 0 7 0


### Zero-delay Sudoku Verifier (zdsv)

Whenever you wish to check your solutions for correctness, the Sudoku
verifier should come in handy. It will report any erroneous or
incomplete solutions it encounters. You can disable unwanted functions
by passing additional parameters via the `zdsv_flags` variable.
See the corresponding source file for more information.

This utility can be altered to only check the format of the input data
without checking for correctness. Use the predefined rule

    make formatverifier

to summon `zdsfv`, the _Zero-delay Sudoku Format Verifier_.
