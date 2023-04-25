
define hello {
  ldc "Hello, world
}
define convert {
  invokestatic [Mm]ethod lib440 java2c
}
define putstr {
  invokestatic [Mm]ethod lib440 putstring
}
define ret {
  iconst_2 OR bipush 2 OR sipush 2 OR ldc 2
  ireturn
}
define seq {
  hello
  convert
  putstr
  ret
}

within main
  NAME String literal
  MATCH hello
  NAME Java to C
  MATCH convert
  NAME putstring
  MATCH putstr
  NAME return 2
  MATCH ret
  NAME Entire sequence
  MATCH seq
done
