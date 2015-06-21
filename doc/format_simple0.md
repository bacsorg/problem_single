# "simple0" format

## Specifications

### Directory tree
Problem in "simple0" is a folder with the following entries:

- *config.ini* -- [Configuration file](#configuration-file)
- *checker/* -- directory with [Checker#TODO]
- *statement/* -- directory with
  [Statement](../../../../bacs_problem/blob/master/doc/statement.md)
- *tests/* -- directory with tests
    - Each test is set of files which names has `test_id.data_id` format
    - Data set of test is set of data_ids
    - Data set must be equal to `{'in'}` or `{'in', 'out'}`.
    - Data sets are equal among different tests
    - Each test file is considered to be either *text* or *binary*
        - File is considered to be *binary* if it is specified in section `[tests]`
          from config.ini
        - File is considered to be *text* if it is specified in section `[tests]`
          from config.ini
        - File is considered to be *text* if it is not specified

## Testing algorithm

1. Solution testing is performed on all tests.
2. Test order is numeric if all test_ids match pattern `'\d+'`
   otherwise order is lexicographical.
3. Testing algorithm is *WHILE_NOT_FAIL*.
4. Solution is executed on specified test.
5. After solution execution checker is executed.

### Configuration file

#### Specifications
- It has [INI format](../../../../bacs_problem/blob/master/doc/ini.md)
- It has "utf8" encoding
- It is divided into several sections
    - **info** section has the following options
        - name -- the name of the problem
        - authors -- the list of authors separated by spaces,
          see [Username](#username)
        - maintainers -- the list of maintainers separated by spaces,
          see [Username](#username)
        - source -- the source of the problem (contest name, championship...)
    - **resource_limits** section has the following options
        - *memory* -- [Fixed point decimal](#fixed-point-decimal),
          see [Memory value](#memory-value)
        - *time* --  [Fixed point decimal](#fixed-point-decimal),
          see [Time value](#time-value)
        - *output* -- [Fixed point decimal](#fixed-point-decimal),
          see [Memory value](#memory-value)
        - *real_time* --  [Fixed point decimal](#fixed-point-decimal),
          see [Time value](#time-value)
    - **files** section has following options: *stdin*, *stdout*, *stderr*.
        - Solution can use up to 3 files corresponding to
          *stdin*, *stdout* and *stderr* data streams.
        - File ids are *stdin*, *stdout*, *stderr*.
        - File with *stdin* id is filled from test file with `data_id=in`.
        - *stdout* and *stderr* may be filled by solution.
        - If option is present no redirections are introduced for file id.
        - If option is not present file redirection is introduced
          and file name is unspecified.
    - **tests** section describes data set of the tests.
      You can specify file format of the data_id.
      `data_id` is one of *in* or *out*.
        - `data_id = text` -- for text files
        - `data_id = binary` -- for binary files

#### Username

Username is used by BACS.WEB.

Username is non-empty string matching the following regular expression: `^[a-zA-Z0-9_-]+$`.

#### Fixed point decimal

Values in configuration files are fixed point decimals
with optional multiple suffix.

Values is defined corresponding to EBNF below.
"multiple unit" non-terminal is defined for specified value types.

Si multiples give 10<sup>k</sup> multiplier.
Si submultiples give 10<sup>-k</sup> multiplier.
Binary multiples give 2<sup>10k</sup> multiplier.

```
value = decimal, multiple unit ;
decimal = unsigned, [dot, unsigned] ;
dot = "." ;
unsigned = digit, {digit} ;
digit = "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9" ;

no multiple = "" ;
si multiple = "da" | "h" | "k" | "M" | "G" | "T" | "P" | "E" | "Z" | "Y" ;
si submultiple = "d" | "c" | "m" | "u" | "n" | "p" | "f" | "a" | "z" | "y" ;
binary multiple = "Ki" | "Mi" | "Gi" | "Ti" | "Pi" | "Ei" | "Zi" | "Yi" ;
```

##### Memory value

```
multiple unit = "" | (multiple, unit) ;
multiple = si multiple | binary multiple | no multiple ;
unit = "B" ;
```

##### Time value

```
multiple unit = "" | (multiple, unit) ;
multiple = si multiple | si submultiple | no multiple ;
unit = "s" ;
```

#### Examples

##### Complicated sample

```ini
[info]
; It is comment
name = Problem name

; Problem is created by "author1" and "author2"
authors = author1 author2

; Here you can specify user names/ids (related to BACS.WEB)
;
; Note that names will be trimmed, so here the following string list is specified:
; ["admin", "contest_admin"]
maintainers = admin contest_admin

source = PTZ summer 2011

[resource_limits]
memory = 256MiB
; 1 second
time = 1s

[files]
; Note that stdin is not specified, so it is redirected from "in" file from test
; stdout will not be redirected
; stderr is redirected to file
stdout = output.txt

[tests]
; Note that we can omit definitions of text files

; This line describes files such as "1.in", "2.in", "3.in" and so on
in = text

; This line describes files "1.out", "2.out" ...
out = text
```

##### Short sample

```ini
[info]
name = Problem name
maintainers = admin

[resource_limits]
memory = 256MiB
time = 1

[files]
stdin = input.txt
stdout = output.txt
```
