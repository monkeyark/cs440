.class public short
.super java/lang/Object


.method public static and : (I)V
    .code stack 2 locals 1
        ; if statement at INPUTS/short.c line 8
        bipush 65
        invokestatic Method lib440 putchar (I)I
        iload_0 ; x
        if_icmpge L1
        bipush 75
        invokestatic Method lib440 putchar (I)I
        iload_0 ; x
        if_icmpge L1
        bipush 85
        invokestatic Method lib440 putchar (I)I
        iload_0 ; x
        if_icmpge L1
        ; expression statement at INPUTS/short.c line 10
        bipush 33
        invokestatic Method lib440 putchar (I)I
        pop
        goto L2
    L1:
        ; expression statement at INPUTS/short.c line 12
        bipush 46
        invokestatic Method lib440 putchar (I)I
        pop
    L2:
        ; expression statement at INPUTS/short.c line 14
        bipush 10
        invokestatic Method lib440 putchar (I)I
        pop
        ; implicit return at INPUTS/short.c line 15
        return
    .end code
.end method


.method public static or : (I)V
    .code stack 2 locals 1
        ; if statement at INPUTS/short.c line 19
        bipush 65
        invokestatic Method lib440 putchar (I)I
        iload_0 ; x
        if_icmpgt L1
        bipush 75
        invokestatic Method lib440 putchar (I)I
        iload_0 ; x
        if_icmpgt L1
        bipush 85
        invokestatic Method lib440 putchar (I)I
        iload_0 ; x
        if_icmple L3
    L1:
        ; expression statement at INPUTS/short.c line 21
        bipush 33
        invokestatic Method lib440 putchar (I)I
        pop
        goto L2
    L3:
        ; expression statement at INPUTS/short.c line 23
        bipush 46
        invokestatic Method lib440 putchar (I)I
        pop
    L2:
        ; expression statement at INPUTS/short.c line 25
        bipush 10
        invokestatic Method lib440 putchar (I)I
        pop
        ; implicit return at INPUTS/short.c line 26
        return
    .end code
.end method


.method public static main : ()I
    .code stack 1 locals 0
        ; expression statement at INPUTS/short.c line 30
        bipush 60
        invokestatic Method short and (I)V
        ; expression statement at INPUTS/short.c line 31
        bipush 70
        invokestatic Method short and (I)V
        ; expression statement at INPUTS/short.c line 32
        bipush 80
        invokestatic Method short and (I)V
        ; expression statement at INPUTS/short.c line 33
        bipush 90
        invokestatic Method short and (I)V
        ; expression statement at INPUTS/short.c line 35
        bipush 60
        invokestatic Method short or (I)V
        ; expression statement at INPUTS/short.c line 36
        bipush 70
        invokestatic Method short or (I)V
        ; expression statement at INPUTS/short.c line 37
        bipush 80
        invokestatic Method short or (I)V
        ; expression statement at INPUTS/short.c line 38
        bipush 90
        invokestatic Method short or (I)V
        ; return at INPUTS/short.c line 39
        iconst_0
        ireturn
        ; implicit return at INPUTS/short.c line 40
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
        invokestatic Method short main ()I
        invokestatic Method java/lang/System exit (I)V
        return
    .end code
.end method
