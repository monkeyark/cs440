
define put0 {
  iload_0
  invokestatic [Mm]ethod lib440 putint .I.V
}

define put1 {
  iload_1
  invokestatic [Mm]ethod lib440 putint .I.V
}


define put33 {
  ldc 33    OR    [bs]ipush 33
  invokestatic [Mm]ethod lib440 putchar .I.I
  pop
}

define put32 {
  ldc 32    OR    [bs]ipush 32
  invokestatic [Mm]ethod lib440 putchar .I.I
  pop
}

within print
  NAME putint i
  MATCH put0
  NAME putchar!
  MATCH put33
  NAME putchar space
  MATCH put32
  NAME putint f
  MATCH put1
done


define const1 {
  iconst_1  OR  [bs]ipush 1  OR  ldc 1
}

define const6 {
  [bs]ipush 6  OR  ldc 6
}

define feq1 {
  const1
  putstatic [Ff]ield fact F I
}

define print1 {
  const1
  getstatic [Ff]ield fact F I
  invokestatic [Mm]ethod fact print .II.V
}

define Fx6a {
  getstatic [Ff]ield fact F I
  const6
  imul
}

define Fx6b {
  const6
  getstatic [Ff]ield fact F I
  imul
}

define setFa {
  dup
  putstatic [Ff]ield fact F I
}

define setFb {
  putstatic [Ff]ield fact F I
  getstatic [Ff]ield fact F I
}

define print6 {
  const6
  Fx6a OR Fx6b
  setFa OR setFb
  invokestatic [Mm]ethod fact print .II.V
}

define retF {
  getstatic [Ff]ield fact F I
  ireturn
}

within main
  NAME F=1
  MATCH feq1
  NAME print(1, F)
  MATCH print1
  NAME print(6, F*=6)
  MATCH print6
  NAME return F
  MATCH retF
done
