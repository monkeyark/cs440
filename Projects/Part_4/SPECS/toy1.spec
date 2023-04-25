
define bc1 {
  iload_1
  iload_2
  imul
}

define bc2 {
  iload_2
  iload_1
  imul
}

define abc1 {
  iload_0
  bc1 OR bc2
  iadd
  ireturn
}

define abc2 {
  bc1 OR bc2
  iload_0
  iadd
  ireturn
}

within func
  MATCH abc1 OR abc2
done

define fcall {
  iconst_3 OR [bs]ipush 3 OR ldc 3
  iconst_4 OR [bs]ipush 4 OR ldc 4
  iconst_5 OR [bs]ipush 5 OR ldc 5
  invokestatic [Mm]ethod toy1 func .III.I
  invokestatic [Mm]ethod lib440 putint .I.V
}

within main
  MATCH fcall
done
