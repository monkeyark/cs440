.class public hello
.super java/lang/Object


.method public static main : ()I
    .code stack 1 locals 0
        ; expression statement at INPUTS/hello.c line 4
        ldc "Hello, world!\n"
        invokestatic Method lib440 java2c (Ljava/lang/String;)[C
        invokestatic Method lib440 putstring ([C)V
        ; return at INPUTS/hello.c line 5
        iconst_2
        ireturn
        ;DEAD    ; expression statement at INPUTS/hello.c line 6
        ;DEAD    ldc "This is dead code\n"
        ;DEAD    invokestatic Method lib440 java2c (Ljava/lang/String;)[C
        ;DEAD    invokestatic Method lib440 putstring ([C)V
        ;DEAD    ; return at INPUTS/hello.c line 7
        ;DEAD    iconst_0
        ;DEAD    ireturn
        ;DEAD    ; implicit return at INPUTS/hello.c line 8
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
