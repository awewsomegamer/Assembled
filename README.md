# Assembled

## What
A column based text editor designed for the purpose of easing the editing of column reliant languages or
column organized languages.

## Compilation
```bash
$ make
```
The above command will compile the source directory into a single executable located at `./assembled.out`.

## Documentation
Documentation files are available at: placewherethingsareavailable.com/documentation/assembled.pdf

### Directories
The layout of the directory is as follows:
```
.
`- src/
   `- editor/ 
   `- interface/
   `- Makefile
`- LICENSE.md
`- Makefile
`- README.md
```
The source directory contains two branches of code: editor and interface. The editor is responsible for handling all underlying functions of the program - managing text buffers, reading and upholding the configuration file, etc. On the other hand, the interface is responsible for interfacing with the user - getting key input, displaying the editor, etc.

## Configuration
The configuration file of the editor is located at `~/.config/assembled/config.cfg`. Themes are located in the `~/.config/assembled/themes/` directory.

## Contribution
For contributions to the main repository, please go about with the standard: fork, new branch, create & commit changes, open pull request procedure.

Ensure that the changes you have made are documented, and to follow the format of the pre-existing code.

```c
#include <stddint.h>

#include "include/my_header.h"

#define MY_CONSTANT (5.234)            // Ensure comments, which are in
#define MY_MACRO    (printf("Words");) // a group are located in one column 

const char *my_str = "My String";
int global_var = 0;

int my_function(int my_var) {
        int a = 0;                     // Make variable definitions clear, on
        int b = 1;                     // separate lines. Ensure a few simple
        int c = 2;                     // comments to inform of the function

        int r = a + b + c;             // of the next bit of code.
        
        MY_MACRO

        return r + MY_CONSTANT;        // Create seperation between the different
                                       // sections of code
}

```