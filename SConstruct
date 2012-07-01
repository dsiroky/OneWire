# vim:syntax=python

PROG = "test"
SRC = Glob("*.c")

MCU = "msp430g2553"
CFLAGS = ["-g", "-Os", "-mmcu=%(MCU)s" % {"MCU": MCU}, "-Wall", "-Wextra"]
LDFLAGS = "-mmcu=%(MCU)s" % {"MCU": MCU}

CC = "msp430-gcc"
OBJDUMP = "msp430-objdump"
OBJCOPY = "msp430-objcopy"
SIZE = "msp430-size"

#####################################################################

import os
env = Environment(ENV = os.environ, CC=CC, CFLAGS=CFLAGS, LINKFLAGS=LDFLAGS)
env.Append(BUILDERS={
    "AsmOut": Builder(action=OBJDUMP + " -DS $SOURCE > $TARGET",
                      src_suffix=".elf", suffix=".lst"),
    "HexOut": Builder(action=OBJCOPY + " -O ihex $SOURCE $TARGET",
                      src_suffix=".elf", suffix=".hex"),
    "Install": Builder(action="mspdebug rf2500 \"prog $SOURCE\"")
  })

prog = env.Program(PROG + ".elf", SRC)
prog_asm = env.AsmOut(prog)
prog_hex = env.HexOut(prog)
Default(prog)

env.Install("install", prog)
