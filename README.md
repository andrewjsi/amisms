# SMS Sender through Asterisk Manager Interface via chan_dongle

AMISMS is a simple command-line tool written in C, which connect to an Asterisk
PBX via Asterisk Manager Interface and send SMS over a USB Huawei GSM/UMTS
modem via chan_dongle channel driver.

AMISMS use the [asynchronous event-driven libamievent
library](https://github.com/andrewjsi/libamievent), which is present as
submodule in this repository.

## Requirements

* gcc, make, binutils
* libev

For Debian users:

    apt-get install build-essential libev-dev

For Gentoo users:

    emerge -av libev

* and access to a working system with Asterisk + chan_dongle + Huawei/ZTE
  dongle + SIM card.

## Install

Clone the amisms repo with all submodules. Optionally run the
`./update_git_submodules script` to sync submodules.

    git clone --recursive git://github.com/andrewjsi/amisms

Compile the source, test and install:

    make
    make test
    sudo make install

## Configuration

The AMISMS search the configuration file at __/home/YOURUSER/.smsrc__ default.
This is an ini-style config file, which contains the AMI server credentials and
the name of the dongle device. Example config file is located in the doc
directory. The simplest configuration looks like this:

```
[device0]
host = localhost        ; hostname or IP address where Asterisk is running
port = 5038             ; port where AMI is listening, see asterisk/manager.conf
username = amiuser      ; name of Asterisk Manager user which have flag 'call'
password = amipassword  ; plaintext password for AMI user
dongle = dongle0        ; dongle name in asterisk/chan_dongle.conf
```

## Usage
```
usage: sms <phone number> <message text>
  or   sms [OPTIONS] <phone number> [message text]

Options
 -v, --verbose               verbose output to stderr
 -f, --flash                 flash SMS
 -s, --stdin                 read message text from stdin
```
## Example

Send a simple message:

    sms +36201234567 "Hello, this is AMISMS"

Send kernel version to mobile phone's display:

    uname -a | sms --flash --stdin +36701234567

## Known bugs

* The international phone number begins with '+' char format does not always
  work.  Try the local country phone number format without the '+' char. It is
  possible that this is caused by an error PDU conversion.

## Caveats

* the PDU routines not perfect, currently only 7bit GSM charset allowed
* AMISMS can send up to 160 characters long messages, multi-part SMS not yet allowed
* only tested on Linux, but we have use this software in production environment

## Todo, Future

* UTF-8 / multibyte message text, ISO, binary, alphabets in PDU
* multi-part SMS
* smooth exit codes
* pretty error and status messages to the user
* easy to parse error and status messages to the calling program
* chan_dongle query functions (dongle state, rssi, etc..)
* manage multiple dongles and AMI servers
* better console management, stdout, stderr, verbose and debug messages
* well-written documentation
* the ability to be integrated with other systems as easy as possible

## Source code

The source code is well written as far as possible. We do not use tabs, instead
of 4 spaces is indented. All identifiers, including variables, function names
and macros written in English, but some comments and commits in Hungarian is,
because we are speaking and thinking in Hungarian. Nevertheless, we try to
write everything in English in the future.

## Contribution

It is an open source project, which is to be better and better. If you have any
ideas or find an error, or get stuck, you can contact us, please file a bug
report or send a pull-request!

## License

[_GPL2_](https://www.gnu.org/licenses/gpl-2.0.html)

(C) Copyright 2010-2014 Andras Jeszenszky, [JSS & Hayer
IT](http://www.jsshayer.hu) All Rights Reserved.
