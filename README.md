# Assembled

## What
A column based text editor designed for the purpose of easing the editing of column reliant languages or
column organized languages.

## Compilation
```bash
$ make
```
The above command will compile the source directory into a single executable located at `./assembled.out`. This executable will use GLIB for string lookups.

If you wish to compile a standalone version (no GLIB), then run the following to generate `./assembled.out`.

``` bash
$ make standalone
```

### Debug

```bash
$ make debug
```

## Documentation
Doxygen documentation is hosted <a href="http://awewsomegaming.net/Assembled/v2/index.html">here</a>.
Documentation can be generated using the following command:
``` bash
$ make documentation
```

### Directories
The layout of the directory is as follows:
```
.
`- docs/
`- src/
   `- editor/ 
   `- include/
      `- editor/
      `- interface/
   `- interface/
`- test/
`- COPYING
`- Doxyfile
`- Makefile
`- README.md
```
The source directory contains two branches of code: editor and interface. The editor is responsible for handling all underlying functions of the program - managing text buffers, reading and upholding the configuration file, reading input from the user and passing it to the interface, etc. On the other hand, the interface is responsible for updating and rendering screens, doing something with user input, etc. The include directory contains the headers for the two branches. The test directory holds all testing files and and docs holds Doxygen documentation.

## Configuration
The configuration file of the editor is located at `~/.config/assembled/config.cfg`. Themes are located in the `~/.config/assembled/themes/` directory.

## License
Assembled is licensed under GPLv2 only, the full license text can be found in the COPYING file.

## Contribution
For contributions to the main repository, please go about with the standard: fork, new branch, create & commit changes, open pull request procedure.

Ensure that the changes you have made are documented, and to follow the format of the pre-existing code.

*Note: where it says STAR, insert another '*' character to make the comment valid doxygen documentation.*

```c
#include <stdint.h>

#include <my_header.h>

#define MY_CONSTANT (5.234)            // Ensure comments, which are in
#define MY_MACRO    (printf("Words");) // a group are located in one column 
/*STAR
* Means something
*/
const char *my_str = "My String";
/*STAR
* Means something
*/
int global_var = 0;

/*STAR
* Does something
*
* Does something but more
* in-depth.
*
* @param my_var - integer that tells function something.
*/
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
