
define condtop {
    L%1:
    iload_0
    ifeq L%2
}

define condbot {
    L%1:
    iload_0
    ifne L2
    iconst_0
    ireturn
}

within main
    NAME While gotos
    MATCH condtop OR condbot
done
