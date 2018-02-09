(This finds the sum
of all the numbers
between 1 and 100
that are divisible by
3 or 5)

(mod{x,y}={x-y{x/y}})
( 0,1,2,  3,4,5,6,7)
(it,x,y,sum,0,0,0,0)
FNC CHECK
    (This is mod 3)
    MDS 2
    SET MDX
    DIV 3
    MUL 3
    NEG
    MDS 1
    MDX 2
    ADD MDX
    IFE 0 JMP YES ENF
    (reset)
    MDS 1
    MDX 0
    SET MDX
    (This is mod 5)
    MDS 2
    SET MDX
    DIV 5
    MUL 5
    NEG
    MDS 1
    MDX 2
    ADD MDX
    IFE 0 JMP YES ELS JMP NO ENF
    (This is the end)
    DEF YES
    MDS 3
    MDX 0
    ADD MDX
    DEF NO
END

FNC RUN
    DEF LOOP
    MDS 0
    MDX 0
    IFL 1000
        MDS 1
        SET MDX
        CAL CHECK
        (iterator)
        MDS 0
        ADD 1
        JMP LOOP
    ENF
    MDS 3
    PRN ANSWER_IS:
    PRN MDS
END

CAL RUN
