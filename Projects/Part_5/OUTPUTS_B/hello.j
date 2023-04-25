.class public hello
.super java/lang/Object


.method public static prints : ([C)V
    .code stack 2 locals 2
        ; expression statement at INPUTS/hello.c line 5
        iconst_0
        istore_1 ; i
        ; begin while loop at INPUTS/hello.c line 6
        goto L1
    L2:
        ; expression statement at INPUTS/hello.c line 7
        aload_0 ; s
        iload_1 ; i
        iinc 1 1
        caload
        invokestatic Method lib440 putchar (I)I
        pop
    L1:
        ; while condition at INPUTS/hello.c line 6
        aload_0 ; s
        iload_1 ; i
        caload
        iconst_0
        if_icmpne L2
        ; end while loop at INPUTS/hello.c line 6
        ; implicit return at INPUTS/hello.c line 9
        return
    .end code
.end method


.method public static main : ()I
    .code stack 1 locals 0
        ; expression statement at INPUTS/hello.c line 13
        ldc "Hello, world!\n"
        invokestatic Method lib440 java2c (Ljava/lang/String;)[C
        invokestatic Method hello prints ([C)V
        ; return at INPUTS/hello.c line 14
        iconst_0
        ireturn
        ; implicit return at INPUTS/hello.c line 15
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
        invokestatic Method hello main ()I
        invokestatic Method java/lang/System exit (I)V
        return
    .end code
.end method
