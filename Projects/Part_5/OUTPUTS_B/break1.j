.class public break1
.super java/lang/Object


.method public static main : ()I
    .code stack 2 locals 1
        ; begin for loop at INPUTS/break1.c line 5
        ; with break label
        ; for initialization at INPUTS/break1.c line 5
        bipush 48
        istore_0 ; a
    L2:
        ; expression statement at INPUTS/break1.c line 7
        iload_0 ; a
        iconst_1
        iadd
        istore_0 ; a
        ; if statement at INPUTS/break1.c line 8
        iload_0 ; a
        bipush 55
        if_icmple L1
        ; break at INPUTS/break1.c line 8
        goto L3
    L1:
        ; expression statement at INPUTS/break1.c line 9
        iload_0 ; a
        invokestatic Method lib440 putchar (I)I
        pop
        ; empty for condition
        goto L2
        ; end for loop at INPUTS/break1.c line 5
    L3:
        ; expression statement at INPUTS/break1.c line 11
        bipush 10
        invokestatic Method lib440 putchar (I)I
        pop
        ; return at INPUTS/break1.c line 12
        iconst_0
        ireturn
        ; implicit return at INPUTS/break1.c line 13
        ;DEAD    return
    .end code
.end method


.method <init> : ()V
    .code stack 1 locals 1
        aload_0
        invokespecial Method java/lang/Object <init> ()V
        return
    .end code
.end method

.method public static main : ([Ljava/lang/String;)V
    .code stack 1 locals 1
        invokestatic Method break1 main ()I
        invokestatic Method java/lang/System exit (I)V
        return
    .end code
.end method
