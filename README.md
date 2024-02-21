
# disktemp - Linux HDD Temperature Checker

disktemp - a simple linux console program, written in C, which relies on and utilizes 'smartmontools' to retrieve the relevant HDD data and outputs it in console.




## Installation

To install `disktemp`, one could download the latest already available executable from here or compile it itself using `source` code provided.

## Compilation

To compile it in linux, `build-essential` package needs to be installed first.

```bash
  apt install build-essential
```
download `disktemp.c` source file and execute:

```bash
  gcc- o disktemp disktemp.c
```

## Requirements

`disktemp` requires `smartmontools` package to be installed prior executing.
It checks if it is already present when executed and offers to install if not.
If user declines `smartmontools` package installation, `disktemp` would terminate itself.

User could install `smartmontools` itself by executing:

```bash
  apt install smartmontools
```

## Running
By default, `disktemp` outputs information for all available disks on host system.

```bash
  ./disktemp
```
User could make it output just a specific disk info by utilizing a `-d` switch:

example:
```bash
  ./disktemp -d /dev/sda
```

Switches available:
````bash
Usage: ./disktemp [-V] [-h | --help] [-d <disk>]

Options:
  -V                Print program version
  -h, --help        Print this help message
  -d <disk>         Check temperature of specified disk
  ````


## Author

- [@TeraflopsBH](https://github.com/TeraflopsBH/)


## License

[GNU AGPLv3](https://choosealicense.com/licenses/agpl-3.0/)


## Badges
[![AGPL License](https://img.shields.io/badge/license-AGPL-blue.svg)](http://www.gnu.org/licenses/agpl-3.0)

