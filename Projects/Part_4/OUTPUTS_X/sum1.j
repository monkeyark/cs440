.class public sum1
.super java/lang/Object


.method public static main : ()I
    .code stack 2 locals 2
        ; expression statement at INPUTS/sum1.c line 5
        ldc "Enter two integers\n"
        invokestatic Method lib440 java2c (Ljava/lang/String;)[C
        invokestatic Method lib440 putstring ([C)V
        ; expression statement at INPUTS/sum1.c line 6
        invokestatic Method lib440 getint ()I
        istore_0 ; a
        ; expression statement at INPUTS/sum1.c line 7
        invokestatic Method lib440 getint ()I
        istore_1 ; b
        ; expression statement at INPUTS/sum1.c line 8
        iload_0 ; a
        invokestatic Method lib440 putint (I)V
        ; expression statement at INPUTS/sum1.c line 9
        ldc " + "
        invokestatic Method lib440 java2c (Ljava/lang/String;)[C
        invokestatic Method lib440 putstring ([C)V
        ; expression statement at INPUTS/sum1.c line 10
        iload_1 ; b
        invokestatic Method lib440 putint (I)V
        ; expression statement at INPUTS/sum1.c line 11
        ldc " = "
        invokestatic Method lib440 java2c (Ljava/lang/String;)[C
        invokestatic Method lib440 putstring ([C)V
        ; expression statement at INPUTS/sum1.c line 12
        iload_0 ; a
        iload_1 ; b
        iadd
        invokestatic Method lib440 putint (I)V
        ; expression statement at INPUTS/sum1.c line 13
        bipush 10
        invokestatic Method lib440 putchar (I)I
        pop
        ; return at INPUTS/sum1.c line 14
        iconst_0
        ireturn
        ;DEAD    ; implicit return at INPUTS/sum1.c line 15
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
        invokestatic Method sum1 main ()I
        invokestatic Method java/lang/System exit (I)V
        return
    .end code
.end method
