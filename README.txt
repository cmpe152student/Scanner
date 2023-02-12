                       ━━━━━━━━━━━━━━━━━━━━━━━━━
                        CMPE 152 PASCAL SCANNER

                                 Team 4
                       ━━━━━━━━━━━━━━━━━━━━━━━━━


Table of Contents
─────────────────

1. Makefile
.. 1. make all (make)
..... 1. make
.. 2. make scanner
.. 3. make clean
.. 4. make test





1 Makefile
══════════

1.1 make all (make)
───────────────────

  The make all command is for rebuilding and retesting this program

  ┌────
  │ make all
  └────

  Includes all of the targets. Enjoy your `./scanner` executable.


1.1.1 make
╌╌╌╌╌╌╌╌╌╌

  This is the default so running

  ┌────
  │ make
  └────

  Should be the same as running `make all`


1.2 make scanner
────────────────

  This builds the scanner program and generates the `./scanner`
  executable

  ┌────
  │ make scanner
  └────


1.3 make clean
──────────────

  ┌────
  │ make clean
  └────

  That deletes `.o` files. `*~` files, and testing files.


1.4 make test
─────────────

  The testing command is pretty simple

  ┌────
  │ make test
  └────

  Runs a `diff` command on hand generated data. `test-in.txt` which is a
  standard pascal program that prints "Hello World" 10 times to the
  screen

  The `//` comment is a line comment in pascal used to verify the token
  labels and are not to be evaluated. These are for help when hand
  testing the data.
