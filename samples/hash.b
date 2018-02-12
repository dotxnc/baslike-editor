(Simple hashing algorithm)
(Written by libeako on SO)
(f{a,b} = s{a+b} + a)
(s{n} = n*{n+1}/2)

FNC S
    MDS 1
    MDX 1
    SET ARG0
    ADD 1
    MUL ARG0
    DIV 2
    RET MDX
END
FNC F
    MDS 0
    SET ARG0
    ADD ARG1
    CAL S <#0>
    MDS 0
    SET RET
    ADD ARG0
    PRN MDS
    RET MDS
END

PRN HASHES:
CAL F <14,32>