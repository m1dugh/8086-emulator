#Intel 8086 Disassembler

##How to use ?

###Building

- Prod build
```bash
$ make
```

- Debug build
```bash
$ make debug
```

###Running

- Running prod binary
```shell
$ ./bin/Release/mmvm -d <file_to_disassemble>
$ ./bin/Release/mmvm <file_to_execute>
```

- Running debug binary
```shell
$ ./bin/Debug/mmvm.debug -d <file_to_disassemble>
$ ./bin/Debug/mmvm.debug <file_to_execute>
```

### About the code

The instructions are first parsed into a structure, and bound to a callback
(implemented in `./src/instructions/implementation/`), and are then ran
all with the same argument.

The main structure of the program is the `emulator` that provides
encapsulation for all the emulated componenents like memory segments
and processor.
All the structures can be found in `./src/models/`.
