.class public fib
.super java/lang/Object


.method public static main : ()I
    .code stack 2 locals 4
        ; expression statement at INPUTS/fib.c line 8
        iconst_0
        istore_1 ; f1
        ; expression statement at INPUTS/fib.c line 9
        iconst_1
        istore_2 ; f2
        ; expression statement at INPUTS/fib.c line 10
        iconst_0
        invokestatic Method lib440 putint (I)V
        ; expression statement at INPUTS/fib.c line 11
        bipush 32
        invokestatic Method lib440 putchar (I)I
        pop
        ; expression statement at INPUTS/fib.c line 12
        bipush 58
        invokestatic Method lib440 putchar (I)I
        pop
        ; expression statement at INPUTS/fib.c line 13
        bipush 32
        invokestatic Method lib440 putchar (I)I
        pop
        ; expression statement at INPUTS/fib.c line 14
        iconst_0
        invokestatic Method lib440 putint (I)V
        ; expression statement at INPUTS/fib.c line 15
        bipush 10
        invokestatic Method lib440 putchar (I)I
        pop
        ; expression statement at INPUTS/fib.c line 16
        bipush 40
        istore_0 ; i
        ; begin while loop at INPUTS/fib.c line 17
        goto L1
    L2:
        ; expression statement at INPUTS/fib.c line 18
        bipush 40
        iinc 0 -1
        iload_0 ; i
        isub
        invokestatic Method lib440 putint (I)V
        ; expression statement at INPUTS/fib.c line 19
        bipush 32
        invokestatic Method lib440 putchar (I)I
        pop
        ; expression statement at INPUTS/fib.c line 20
        bipush 58
        invokestatic Method lib440 putchar (I)I
        pop
        ; expression statement at INPUTS/fib.c line 21
        bipush 32
        invokestatic Method lib440 putchar (I)I
        pop
        ; expression statement at INPUTS/fib.c line 22
        iload_2 ; f2
        invokestatic Method lib440 putint (I)V
        ; expression statement at INPUTS/fib.c line 23
        bipush 10
        invokestatic Method lib440 putchar (I)I
        pop
        ; expression statement at INPUTS/fib.c line 24
        iload_1 ; f1
        iload_2 ; f2
        iadd
        istore_3 ; f3
        ; expression statement at INPUTS/fib.c line 25
        iload_2 ; f2
        istore_1 ; f1
        ; expression statement at INPUTS/fib.c line 26
        iload_3 ; f3
        istore_2 ; f2
    L1:
        ; while condition at INPUTS/fib.c line 17
        iload_0 ; i
        ifne L2
        ; end while loop at INPUTS/fib.c line 17
        ; return at INPUTS/fib.c line 28
        iconst_0
        ireturn
        ; implicit return at INPUTS/fib.c line 29
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
        invokestatic Method fib main ()I
        invokestatic Method java/lang/System exit (I)V
        return
    .end code
.end method
