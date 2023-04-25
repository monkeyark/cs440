
define lseq1 {
  L%1:
  aload_0
  iload_1
  caload
  iconst_0
  if_icmpne L%2
}

define lseq2 {
  aload_0
  iload_1
  caload
  iconst_0
  if_icmpeq L%1
}

within prints
    NAME loop
    MATCH lseq1
done

within main
    NAME foo
    MATCH .
done
