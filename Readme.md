#w5p#
##wordle list packer for memory constrained systems##

This is a small C program to pack 5-letter ASCII word lists into
a binary file of 5-bits per letter.

A bitmask of %11111 is applied to the each character and then
 each bit is shifted into the output buffer without padding.

This allows the Wordle valids list to fit into 40k, making
it possible to load this into an 8-bit machine's 16-bit
address space.

To build, type `make`

Example use:

`./w5p valid-wordle-words.txt packed.bin`

That's it, that's the program. I've included the "official" input
list and the results of a run of the program- so no need to even
run it if you're after the output.

*Note:
Since it processes these files in RAM, the program itself requires
approximately 100k of heap- so it would be difficult to run this
on the target system. And if you have more than 64k, then you probably
don't have a need for this.*




Questions to Ryan Brooks / ryan@hack.net
Jan, 2022
BSD Licensed.
